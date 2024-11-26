#include "MidiParser.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <stop_token>
#include <thread>
#include <chrono>
#include <mutex>
#include <ranges>
#include <print>
#include <string>

#include "midi_globals.h"
#include "helpers.h"

#include "minitrace.h"

////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

template <typename Type>
void flip_bytes_inplace(Type& origValue) {
	constexpr size_t TypeSize = sizeof(Type);

	char* OrigBytes = reinterpret_cast<char*>(&origValue);

	for(size_t i = 0; i < TypeSize / 2; ++i) {
		const char lastByte = OrigBytes[(TypeSize - 1) - i]; 
		OrigBytes[(TypeSize - 1) - i] = OrigBytes[i];
		OrigBytes[i] = lastByte;
	}
}

//Time Signature is needed for `division`
inline long double convert_dt_to_ms(const uint64_t current_tick, const uint64_t tempo, const uint64_t division) {
	return (double)current_tick * ((double)tempo / (double)division) * 1000.0;
}

/*
	http://www.ccarh.org/courses/253/handout/vlv/
	Definition of Variable Length Values
	
	A MIDI file Variable Length Value is stored in bytes.
	Each byte has two parts: 7 bits of data and 1 continuation bit.
	The highest-order bit is set to 1 if there is another byte of the number to follow.
	The highest-order bit is set to 0 if this byte is the last byte in the VLV.

	To recreate a number represented by a VLV, first you remove the continuation bit and
	then concatenate the leftover bits into a single number.

	To generate a VLV from a given number, break the number up into 7 bit units and
	then apply the correct continuation bit to each byte.

	In theory, you could have a very long VLV number which was quite large;
	however, in the standard MIDI file specification, the maximum length of
	a VLV value is 5 bytes, and the number it represents can not be larger than 4 bytes. 
*/
uint64_t parse_vlv(std::ifstream& file) {
	uint64_t value = 0;
	uint8_t  byte  = 0;
	unsigned char fetched_byte = 0;

	while(true) {
		byte++;
		file.read(reinterpret_cast<char*>(&fetched_byte), sizeof(char));
		
		// Append the byte into our variable
		value <<= 7;
		value |= fetched_byte & 0x7f;
		
		if(!(fetched_byte & 0x80) || byte >= 4) {// Potentially can remove byte restriction for longer Delta-Times
			break;
		}
	}
	
	return value;
}


uint64_t notes_count = 0;
MIDI_ParsedData MidiParser::midi_header = MIDI_ParsedData();
midi MidiParser::parsed_midi = midi();
std::mutex MidiParser::g_track_write_mutex = std::mutex();

void MidiParser::worker_TrackRead(const std::stop_token& stop_token, const wchar_t* file_path, const uint16_t track_index) {
	#if MTR_ENABLED
		char thread_name[30];
		(void)sprintf_s(thread_name, "Track Thread %i", track_index);
		MTR_META_THREAD_NAME(thread_name);
	#endif

	std::ifstream midi_file(file_path, std::ifstream::binary);
	if(!midi_file.good()) {
		debug::printf("Couldn't open the file\n");
		return;
	}

	midi_file.ignore(14); // Skip the MIDI header part
	
	debug::printf("[Track %d] Reading has started.\n", track_index);

	Track* new_track = nullptr;
	if(midi_header.type != 0) {
		new_track = new Track();
	}
	
	char     track_chunk_ID[5];
	uint32_t track_chunk_size = 0;
	uint64_t absolute_time = 0;
	uint8_t  event;
	uint8_t	 currently_skipped_track = 0;

	if (midi_header.type != 0) {
		while(currently_skipped_track < track_index){
			if(stop_token.stop_requested()) {
				return;
			}

			// Skip the track label
			midi_file.ignore(4);
			// Get chunk size to skip
			uint32_t track_chunk_size_toSkip;
			midi_file.read(reinterpret_cast<char*>(&track_chunk_size_toSkip), sizeof(char) * 4);
			flip_bytes_inplace(track_chunk_size_toSkip);
			midi_file.ignore(track_chunk_size_toSkip);

			currently_skipped_track++;
		}
	}


	// Reading track label.
	midi_file.read(track_chunk_ID, sizeof(char) * 4);
	track_chunk_ID[4] = '\0';
	
	// Check if we are at a valid track after skipping.
	if(strcmp(track_chunk_ID, "MTrk") != 0) {
		debug::printf("Found chunk '%s' at %d \n", track_chunk_ID, static_cast<uint64_t>(midi_file.tellg()));
		assert(false && "There is no track after skipping, or MIDI is corrupted.\n");
		return;
	}
	
	midi_file.read(reinterpret_cast<char*>(&track_chunk_size), sizeof(char) * 4);
	flip_bytes_inplace(track_chunk_size);
	
	debug::printf("Track chunk size: %u\n", track_chunk_size);

	if (new_track && midi_header.type != 0) {
		// MTR_SCOPE("Track Read", "EVENT VECTORS RESERVE");
		// Reseves vector sizes to the approximate size for the midi.
		const uint64_t to_reserve = track_chunk_size/5;
		new_track->events[globals::midi::event_names_by_code.at(0x80)].reserve(to_reserve);
		new_track->events[globals::midi::event_names_by_code.at(0x90)].reserve(to_reserve);
	}
	
	// Start Events reading loop
	bool is_reading_track = true;
	while(is_reading_track) {
		if(stop_token.stop_requested()) {
			debug::printf("Track Read %d has stopped\n", track_index);
			return;
		}
		
		absolute_time += parse_vlv(midi_file);
		parsed_midi.length = absolute_time;

		// Get event
		midi_file.read(reinterpret_cast<char*>(&event), sizeof(char));
		
		if(event == 0xFF) { // Meta events
			char meta_event_type;
			
			midi_file.read(&meta_event_type, sizeof(char));
			
			MetaEvent* new_meta_event = new MetaEvent;
			new_meta_event->type = meta_event_type;
			new_meta_event->name = globals::midi::meta_event_names.at(meta_event_type);
			new_meta_event->time = absolute_time;
			
			switch(meta_event_type) {
				case 0x00: {	// Sequence Number
					uint8_t MSB = 0; // [0, 255]
					uint8_t LSB = 0; // [0, 255]

					midi_file.ignore(); // Ignoring size byte
					midi_file.read(reinterpret_cast<char*>(&MSB), sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&LSB), sizeof(char));
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				// Text based events
				case 0x01:		// Text
				case 0x02:		// Copyright Notice
				case 0x03:		// Sequence/Track Name
				case 0x04:		// Instrument Name
				case 0x05:		// Lyrics
				case 0x06:		// Marker 
				case 0x07:		// Cue Point
				case 0x7F: {	// Sequencer Specific
					const uint64_t  meta_event_size = parse_vlv(midi_file);
					char*           text            = new char[meta_event_size+1];
					
					midi_file.read(text, sizeof(char) * meta_event_size);
					text[meta_event_size] = '\0';
					
					new_meta_event->text = text;
					
					delete[] text;
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x20: {	// MIDI Channel Prefix
					uint64_t& channel = new_meta_event->value1; // [0, 15]

					midi_file.ignore(); // Ignoring size byte
					midi_file.read(reinterpret_cast<char*>(&channel), sizeof(char));
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				// MICROSECONDS_PER_MINUTE = 60000000 BPM = MICROSECONDS_PER_MINUTE / MPQNMPQN = MICROSECONDS_PER_MINUTE / BPM
				// If not provided, the tempo should be set to 120 BPM.
				case 0x51: {	// Set Tempo
					uint64_t& tempo = new_meta_event->value1; // 0-8355711

					midi_file.ignore(); // Ignoring size byte
					midi_file.read(reinterpret_cast<char*>(&tempo), sizeof(char) * 3);
					flip_bytes_inplace(tempo);
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x54: {                                  // SMPTE Offset
					uint64_t& hour   = new_meta_event->value1; // [0-23]
					uint64_t& min    = new_meta_event->value2; // [0-59]
					uint64_t& sec    = new_meta_event->value3; // [0-59]
					uint64_t& fr     = new_meta_event->value4; // [0-30]
					uint64_t& sub_fr = new_meta_event->value5; // [0-99]
					
					midi_file.ignore(); // Ignoring size value
					midi_file.read(reinterpret_cast<char*>(&hour),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&min),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&sec),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&fr),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&sub_fr),sizeof(char));

					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x58: { // Time Signature
					// PROFILE_SCOPE("Time Signature");
					uint64_t& number      = new_meta_event->value1; // [0-255]
					uint64_t& denominator = new_meta_event->value2; // [0-255]
					uint64_t& metro       = new_meta_event->value3; // [0-255]
					uint64_t& _32nds      = new_meta_event->value4; // [1-255]
					//
					midi_file.ignore(); // Ignoring size byte
					midi_file.read(reinterpret_cast<char*>(&number),		sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&denominator),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&metro),			sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&_32nds),		sizeof(char));

					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x59: {	// Key Signature
					// PROFILE_SCOPE("Key Signature");
					int8_t  key;	//[-7, 7]
					bool	scale;	//major/minor

					midi_file.ignore(); // Ignoring size byte
					midi_file.read(reinterpret_cast<char*>(&key),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&scale),	sizeof(char));

					new_meta_event->value1 = key + 7; 
					new_meta_event->value2 = scale;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x2F: {	// END OF TRACK
					// Skip last 0x0 of the ending pattern
					// pattern: 0xFF 0x2F 0x00
					midi_file.ignore(); // Ignoring size byte
					is_reading_track = false;
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				default: {
					debug::printf("This Meta Event is undefined: %x\n", meta_event_type);
				} 
			}

			// MTR_SCOPE("MIDI Events", "META EVENT SAVE");
			if(midi_header.type == 0) {
				new_track = parsed_midi.tracks[0];
			}
			new_track->meta_events[new_meta_event->name].push_back(new_meta_event);
		}
		else if ((event & 0xF0) == 0xF0) // Sys Ex events
		{
			// NOTE: we just ignore the data for now.
			// SysExEvent new_sys_ex_event;
			// new_sys_ex_event.type = event;
			// new_sys_ex_event.name = "SysEx";
			// new_sys_ex_event.time = absolute_time;
		
			int64_t sysex_size = 0;
			midi_file.read(reinterpret_cast<char*>(&sysex_size), sizeof(char));
			midi_file.ignore(sysex_size - 1);	// Skip packet
			midi_file.ignore();					// Skip ending byte.
		}
		else // MIDI events
		{
			// MTR_SCOPE("MIDI Events", "EVENT READ");
			
			MIDI_Event* new_event = new MIDI_Event;
			new_event->type    = event;
			new_event->name    = globals::midi::event_names_by_code.at(event & 0xF0);
			new_event->time    = absolute_time;
			new_event->channel = event & 0x0F; // Channel: [0-15]

			if(midi_header.type == 0) {
				new_track = parsed_midi.tracks[new_event->channel];
			}

			switch(event & 0xF0) {
				case 0x80: { // Note OFF
					// MTR_SCOPE("MIDI Events", "Note OFF");
					uint8_t note;		// [0-127]
					uint8_t vel;		// [0-127]

					midi_file.read(reinterpret_cast<char*>(&note),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&vel),	sizeof(char));
					
					new_event->value1 = note;
					new_event->value2 = vel;

					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0x90: { // Note ON
					// MTR_SCOPE("MIDI Events", "Note ON");
					uint8_t note;		// [0-127]
					uint8_t vel;		// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&note),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&vel),	sizeof(char));

					new_event->value1 = note;
					new_event->value2 = vel;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0xA0: { // Note Aftertouch
					// MTR_SCOPE("TRACK READ", "Note Aftertouch");
					uint8_t note;		// [0-127]
					uint8_t amount;		// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&note),		sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&amount),	sizeof(char));

					new_event->value1 = note;
					new_event->value2 = amount;

					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0xB0: { // Controller
					// MTR_SCOPE("TRACK READ", "Controller");
					/*	CONTROLLER TYPES: 
						0		(0x00) 			Bank Select
						1		(0x01) 			Modulation
						2		(0x02) 			Breath Controller
						4		(0x04) 			Foot Controller
						5		(0x05) 			Portamento Time
						6		(0x06) 			Data Entry (MSB)
						7		(0x07) 			Main Volume
						8		(0x08) 			Balance
						10		(0x0A) 			Pan
						11		(0x0B) 			Expression Controller
						12		(0x0C) 			Effect Control 1
						13		(0x0D) 			Effect Control 2
						16-19	(0x10-0x13) 	General-Purpose Controllers 1-4
						32-63	(0x20-0x3F) 	LSB for controllers 0-31
						64		(0x40) 			Damper pedal (sustain)
						65		(0x41) 			Portamento
						66		(0x42) 			Sostenuto
						67		(0x43) 			Soft Pedal
						68		(0x44) 			Legato Footswitch
						69		(0x45) 			Hold 2
						70		(0x46) 			Sound Controller 1 (default: Timber Variation)
						71		(0x47) 			Sound Controller 2 (default: Timber/Harmonic Content)
						72		(0x48) 			Sound Controller 3 (default: Release Time)
						73		(0x49) 			Sound Controller 4 (default: Attack Time)
						74-79	(0x4A-0x4F) 	Sound Controller 5-10
						80-83	(0x50-0x53) 	General-Purpose Controllers 5-8
						84		(0x54) 			Portamento Control
						91		(0x5B) 			Effects 1 Depth (formerly External Effects Depth)
						92		(0x5C) 			Effects 2 Depth (formerly Tremolo Depth)
						93		(0x5D) 			Effects 3 Depth (formerly Chorus Depth)
						94		(0x5E) 			Effects 4 Depth (formerly Celeste Detune)
						95		(0x5F) 			Effects 5 Depth (formerly Phaser Depth)
						96		(0x60) 			Data Increment
						97		(0x61) 			Data Decrement
						98		(0x62) 			Non-Registered Parameter Number (LSB)
						99		(0x63) 			Non-Registered Parameter Number (MSB)
						100		(0x64) 			Registered Parameter Number (LSB)
						101		(0x65) 			Registered Parameter Number (MSB)
						121-127	(0x79-0x7F) 	Mode Messages
					*/
					uint8_t controller_type;	// [0-127]
					uint8_t value;				// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&controller_type),	sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&value),				sizeof(char));

					new_event->value1 = controller_type;
					new_event->value2 = value;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0xC0: { // Program Change
					// MTR_SCOPE("TRACK READ", "Program Change");
					uint8_t program_number;	// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&program_number), sizeof(char));
					
					new_event->value1 = program_number;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0xD0: { // Channel Aftertouch
					// MTR_SCOPE("TRACK READ", "Channel Aftertouch");
					uint8_t amount;		// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&amount), sizeof(char));

					new_event->value1 = amount;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				case 0xE0: { // Pitch Bend
					// MTR_SCOPE("TRACK READ", "Pitch Bend");
					uint8_t LSB;		// [0-127]
					uint8_t MSB;		// [0-127]
					
					midi_file.read(reinterpret_cast<char*>(&LSB), sizeof(char));
					midi_file.read(reinterpret_cast<char*>(&MSB), sizeof(char));
					
					new_event->value1 = LSB;
					new_event->value2 = MSB;
					
					break;
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
				default: {
					debug::printf("This Event is undefined: %x\n", event);
				} 
			}
			
			{
				// MTR_SCOPE("TRACK READ", "Event Save");
				new_track->channel = new_event->channel;
				new_track->events[new_event->name].push_back(new_event);					
			}
		}
	}
	
	////////////////////////////////////////////////////////////
	////                FINALIZING TRACK                    ////
	////////////////////////////////////////////////////////////
	if(new_track->meta_events.contains("Track Name")) {
		new_track->name = new_track->meta_events["Track Name"][0]->text;
	}
	
	// Sorting by time
	for(auto& events : new_track->events | std::views::values) {
		std::ranges::sort(
			events,
			[](const MIDI_Event* A, const MIDI_Event* B) {
			  return A->time < B->time;
			}
		);
	}
	
	// Sorting by time
	for(auto& events : new_track->meta_events | std::views::values) {
		std::ranges::sort(
			events,
			[](const MetaEvent* A, const MetaEvent* B) {
			  return A->time < B->time;
			}
		);
	}
	
	////////////////////////////////////////////////////////////
	
	debug::printf("Track %s is Done loading! Waiting for mutex.\n\n", new_track->name.c_str());
	// MTR_SCOPE("Track Read", "MUTEX UNLOCK");
	std::lock_guard guard(g_track_write_mutex);
	notes_count += new_track->events[globals::event_names::NoteOn].size();
	parsed_midi.tracks.emplace_back(new_track);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MidiParser::Read(wchar_t* file_path) {
	// // MTR_init("MidiParse_Trace.json");
	// // MTR_META_PROCESS_NAME("MIDI Read");
	// // MTR_META_THREAD_NAME("[MIDI Read] Main Thread");
	// // MTR_BEGIN_FUNC();

	//////////////////////////////////////////
	////         GETTING FILE NAME        ////
	//////////////////////////////////////////
	
	debug::printf("Parsing: %s\n", file_path);
	
	uint16_t     offset   = 0;
	const size_t path_len = wcslen(file_path);
	while(offset < path_len) {
		const wchar_t symbol = file_path[path_len-offset];
		if(symbol == '/') {
			offset--;
			break;
		}
		offset++;
	}

	wchar_t* found_file_name = file_path+(path_len-offset);
	
	debug::printf("File name: %s\n", found_file_name);

	////////////////////////

	midi_header = MIDI_ParsedData();
	parsed_midi.name = found_file_name;
	
	//////////////////////////////////////////
	////            HEADER PART           ////
	//////////////////////////////////////////
	
	const auto t1 = std::chrono::high_resolution_clock::now();
	{
		// MTR_SCOPE("MIDI", "HEADER PARSE");
		
		std::ifstream midi_file(file_path, std::ifstream::binary);
		if(!midi_file.good()){
			return false;
		}
		
		// Checking if it's a MIDI file
		char MIDI_ID[5];
		midi_file.read(MIDI_ID, sizeof(char) * 4);
		MIDI_ID[4] = '\0';
		if(strcmp(MIDI_ID, "MThd") != 0) {
			debug::printf("The file is not a midi.\n");
			return false;
		}

		//////////////////////////////////////////

		// Get header chunk size
		midi_file.read(reinterpret_cast<char*>(&midi_header.header_size), sizeof(char) * 4);
		flip_bytes_inplace(midi_header.header_size);
		debug::printf("Header size: %u\n", midi_header.header_size);
		
		//////////////////////////////////////////

		// Get Type
		midi_file.read(reinterpret_cast<char*>(&midi_header.type), sizeof(char) * 2);
		flip_bytes_inplace(midi_header.type);
		debug::printf("Type: %d\n", midi_header.type);

		//////////////////////////////////////////

		// Get num of tracks
		midi_file.read(reinterpret_cast<char*>(&midi_header.number_of_tracks), sizeof(char) * 2);
		flip_bytes_inplace(midi_header.number_of_tracks);
		debug::printf("Number of tracks: %d\n", midi_header.number_of_tracks);

		//////////////////////////////////////////

		// Get time division
		// TODO: Translate data into usable thing
		midi_file.read(reinterpret_cast<char*>(&midi_header.time_division), sizeof(char) * 2);
		// flip_bytes_inplace(midi_header.time_division);
		debug::printf("Time division: %x\n", midi_header.time_division);

		if(midi_header.time_division & 0x8000) {
			// Ticks per Quarter note
			midi_header.ticks_per_qn = midi_header.time_division & ~0x8000;
		} else {
			// SMPTE Format
			midi_header.SMPTE_format	= (short)midi_header.time_division & 0b011111100000000;
			midi_header.ticks_per_frame = midi_header.time_division & 0b000000011111111;
		}
	}

	//////////////////////////////////////////
	////            TRACKS PART           ////
	//////////////////////////////////////////

	debug::printf("\n=======================\nTracks\n=======================\n\n");
	
	parsed_midi.name = found_file_name;
	{
		std::vector<std::unique_ptr<std::jthread>> threads;

		const uint16_t number_of_tracks = (midi_header.type == 0) ? 1 : midi_header.number_of_tracks;
		if (midi_header.type == 0) {
			parsed_midi.tracks.resize(16, {});
			for(auto& track : parsed_midi.tracks) {
				track = new Track();
			}
		}
		for(uint16_t track_index = 0; track_index < number_of_tracks; ++track_index) {
			threads.push_back(std::make_unique<std::jthread>(
				worker_TrackRead,
				file_path, track_index
			));
		}
	
		for(const auto& thread : threads) {
			thread->join();
		}
	}
	const auto t2 = std::chrono::high_resolution_clock::now();
	
	debug::printf("=====================\n");
	debug::printf("MIDI loading is Done!\n");
	debug::printf("=====================\n");
	debug::printf("The loading took %f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
	debug::printf("Amount of notes: %d\n", notes_count);

	#if _DEBUG
	// for(auto& track : parsed_midi.tracks) {
	// 	debug::printf("Channel %d\n", track->channel);
	// }
	// for(auto& track : parsed_midi.tracks) {
	// 	debug::print("Track's name: {}", track->name.c_str());
	// 	for(const auto& note : track->events["Note ON"]) {
	// 		// const long double note_time = convert_dt_to_ms(
	// 		// 	note->time,
	// 		// 	track->meta_events["Set Tempo"][0]->value1,
	// 		// 	midi_header.time_division
	// 		// );
	// 		debug::print("\tHas note {} at {}", globals::midi::note_names[note->value1 % 12], note->time);
	// 	}
	// }
	#endif

	// // MTR_END_FUNC();
	// // MTR_shutdown();
	return true;
}
