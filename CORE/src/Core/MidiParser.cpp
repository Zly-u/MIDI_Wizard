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

namespace MPC = MidiParserConvertors;

uint64_t        notes_count = 0;
MIDI_ParsedData MidiParser::midi_header;
midi            MidiParser::parsed_midi;
std::mutex      MidiParser::g_track_write_mutex = std::mutex();

void MidiParser::worker_TrackRead(const std::stop_token& stop_token, const wchar_t* file_path, const uint16_t track_index) {
	using namespace globals::midi;
	
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
			MPC::flip_bytes_inplace(track_chunk_size_toSkip);
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
	MPC::flip_bytes_inplace(track_chunk_size);
	
	debug::printf("Track chunk size: %u\n", track_chunk_size);

	if (new_track && midi_header.type != 0) {
		// MTR_SCOPE("Track Read", "EVENT VECTORS RESERVE");
		// Reseves vector sizes to the approximate size for the midi.
		const uint64_t to_reserve = track_chunk_size/5;
		new_track->events[event_names_by_code.at(event_codes::NoteOff)].reserve(to_reserve);
		new_track->events[event_names_by_code.at(event_codes::NoteOn)].reserve(to_reserve);
	}

	// Start Events reading loop
	bool is_reading_track = true;
	while(is_reading_track) {
		if(stop_token.stop_requested()) {
			debug::printf("Track Read %d has stopped\n", track_index);
			return;
		}
		
		absolute_time += MPC::parse_vlv(midi_file);
		parsed_midi.length = absolute_time;

		// Get event
		midi_file.read(reinterpret_cast<char*>(&event), sizeof(char));
		
		if(event == 0xFF) { // Meta events
			char meta_event_type;
			
			midi_file.read(&meta_event_type, sizeof(char));

			if(midi_header.type == 0) {
				new_track = parsed_midi.tracks[0].get();
			}

			auto me_name = meta_event_names.at(meta_event_type);
			MetaEvent* new_meta_event = new_track->meta_events[me_name].emplace_back(std::make_unique<MetaEvent>()).get();
			new_meta_event->type = meta_event_type;
			new_meta_event->name = me_name;
			new_meta_event->time = absolute_time;

			switch(meta_event_type) {
				case meta_event_codes::SequenceNumber: {
					MPC::Convert<meta_event_codes::SequenceNumber>::decode(midi_file, new_meta_event);
					break;
				}
				// Text based events
				case meta_event_codes::Text:
				case meta_event_codes::CopyrightNotice:
				case meta_event_codes::TrackName:
				case meta_event_codes::InstrumentName:
				case meta_event_codes::Lyrics:
				case meta_event_codes::Marker:
				case meta_event_codes::CuePoint:
				case meta_event_codes::SequencerSpecific: {
					MPC::Convert<meta_event_codes::Text>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::MIDIChannelPrefix: {
					MPC::Convert<meta_event_codes::MIDIChannelPrefix>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::SetTempo: {
					MPC::Convert<meta_event_codes::SetTempo>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::SMPTEOffset: {
					MPC::Convert<meta_event_codes::SMPTEOffset>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::TimeSignature: {
					MPC::Convert<meta_event_codes::TimeSignature>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::KeySignature: {
					MPC::Convert<meta_event_codes::KeySignature>::decode(midi_file, new_meta_event);
					break;
				}
				case meta_event_codes::EndOfTrack: {
					MPC::Convert<meta_event_codes::EndOfTrack>::decode(midi_file, new_meta_event);
					is_reading_track = false;
					break;
				}
				default: {
					debug::printf("This Meta Event is undefined: %x\n", meta_event_type);
				} 
			}

			if(midi_header.type == 0) {
				new_track = parsed_midi.tracks[0].get();
			}
		}
		else if ((event & 0xF0) == 0xF0) // Sys Ex events
		{
			// NOTE: we just ignore the data for now.
			MPC::Convert<0xF0>::decode(midi_file, nullptr);
		}
		else // MIDI events
		{
			const uint8_t event_code = event & 0xF0;
			const uint8_t event_channel = event & 0x0F;

			if(midi_header.type == 0) {
				new_track = parsed_midi.tracks[event_channel].get();
			}

			auto e_name = event_names_by_code[event_code];
			MIDI_Event* new_event = new_track->events[e_name].emplace_back(std::make_unique<MIDI_Event>()).get();
			new_event->type    = event;
			new_event->name    = e_name;
			new_event->time    = absolute_time;
			new_event->channel = event_channel; // Channel: [0-15]

			switch(event_code) {
				case event_codes::NoteOff: {
					MPC::Convert<event_codes::NoteOff>::decode(midi_file, new_event);
					break;
				}
				case event_codes::NoteOn: {
					MPC::Convert<event_codes::NoteOn>::decode(midi_file, new_event);
					break;
				}
				case event_codes::NoteAftertouch: {
					MPC::Convert<event_codes::NoteAftertouch>::decode(midi_file, new_event);
					break;
				}
				case event_codes::Controller: {
					MPC::Convert<event_codes::Controller>::decode(midi_file, new_event);
					break;
				}
				case event_codes::ProgramChange: {
					MPC::Convert<event_codes::ProgramChange>::decode(midi_file, new_event);
					break;
				}
				case event_codes::ChannelAftertouch: {
					MPC::Convert<event_codes::ChannelAftertouch>::decode(midi_file, new_event);
					break;
				}
				case event_codes::PitchBend: {
					MPC::Convert<event_codes::PitchBend>::decode(midi_file, new_event);
					break;
				}
				default: {
					debug::printf("This Event is undefined: %x\n", event);
				} 
			}
			
			new_track->channel = new_event->channel;
		}
	}
	
	////////////////////////////////////////////////////////////
	////                FINALIZING TRACK                    ////
	////////////////////////////////////////////////////////////

	if(new_track->meta_events.contains("Track Name")) {
		new_track->name = new_track->meta_events["Track Name"][0]->text;
	}
	
	// Sorting by time
	for(auto& [_, events] : new_track->events) {
		std::ranges::sort(
			events,
			[](const std::unique_ptr<MIDI_Event>& A, const std::unique_ptr<MIDI_Event>& B) {
			  return A->time < B->time;
			}
		);
	}
	
	// Sorting by time
	for(auto& [_, events] : new_track->meta_events) {
		std::ranges::sort(
			events,
			[](const std::unique_ptr<MetaEvent>& A, std::unique_ptr<MetaEvent>& B) {
			  return A->time < B->time;
			}
		);
	}
	
	////////////////////////////////////////////////////////////
	
	debug::printf("Track %s is Done loading! Waiting for mutex.\n\n", new_track->name.c_str());
	// MTR_SCOPE("Track Read", "MUTEX UNLOCK");
	std::lock_guard guard(g_track_write_mutex);
	notes_count += new_track->events[event_names::NoteOn].size();
	if (midi_header.type != 0) {
		parsed_midi.tracks.emplace_back(new_track);
	}
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
	
	uint16_t offset = 0;
	const size_t path_len = wcslen(file_path);
	while(offset < path_len) {
		const wchar_t symbol = file_path[path_len-offset];
		if(symbol == '\\' || symbol == '/') {
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
		MPC::flip_bytes_inplace(midi_header.header_size);
		debug::printf("Header size: %u\n", midi_header.header_size);
		
		//////////////////////////////////////////

		// Get Type
		midi_file.read(reinterpret_cast<char*>(&midi_header.type), sizeof(char) * 2);
		MPC::flip_bytes_inplace(midi_header.type);
		debug::printf("Type: %d\n", midi_header.type);

		//////////////////////////////////////////

		// Get num of tracks
		midi_file.read(reinterpret_cast<char*>(&midi_header.number_of_tracks), sizeof(char) * 2);
		MPC::flip_bytes_inplace(midi_header.number_of_tracks);
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
			for(size_t i = 0; i < 16; i++) {
				auto new_track = parsed_midi.tracks.emplace_back(std::make_shared<Track>());
				new_track->name = std::string("Channel ") + std::to_string(i);
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

		// Remove empty tracks for auto generated channels
		if (midi_header.type == 0) {
			for(auto It = parsed_midi.tracks.begin(); It != parsed_midi.tracks.end();) {
				Track* track = It->get();

				if(!track->events.empty()) {
					++It;
				} else {
					It = parsed_midi.tracks.erase(It);
				}
			}
		}
	}
	const auto t2 = std::chrono::high_resolution_clock::now();
	
	debug::printf("=====================\n");
	debug::printf("MIDI loading is Done!\n");
	debug::printf("=====================\n");
	debug::printf("The loading took %f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
	debug::printf("Amount of notes: %d\n", notes_count);

	debug::printf("First track ptr: %p\n", parsed_midi.tracks[0].get());
	debug::printf("Last track ptr: %p\n", (parsed_midi.tracks.end()-1)->get());

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

	// MTR_END_FUNC();
	// MTR_shutdown();
	return true;
}
