﻿#include "midi.h"

#include <cassert>
#include <fstream>

////////////////////////////////////////////////////////////////////////

struct MIDI_ParsedData {
	char           chunk_ID[5];
	uint32_t       header_size      = 0;
	unsigned short type             = 0; // 0-2
	unsigned short number_of_tracks = 1; // 1-65535
	unsigned short time_division    = 0; // TODO: time_division
};

struct EventsData {
	char* midi;
	char* system_exclusive;
	char* meta;
};

struct EventData {
	uint32_t deltatime;
};

struct TrackParsedData {
	uint32_t chunk_size = 0;

	std::string name;
	
	EventsData events;
};


std::map<uint8_t, std::string> meta_event_names = {
	{0x00, "Sequence Number"},
	{0x01, "Text"},
	{0x02, "Copyright Notice"},
	{0x03, "Track Name"},
	{0x04, "Instrument Name"},
	{0x05, "Lyrics"},
	{0x06, "Marker"},
	{0x07, "Cue Point"},
	{0x20, "MIDI Channel Prefix"},
	{0x2F, "END OF TRACK"},
	{0x51, "Set Tempo"},
	{0x54, "SMPTE Offset"},
	{0x58, "Time Signature"},
	{0x59, "Key Signature"},
	{0x7F, "Sequencer Specific"},
};
std::map<uint8_t, std::string> event_names = {
	{0x8, "Note OFF"},
	{0x9, "Note ON"},
	{0xA, "Note Aftertouch"},
	{0xB, "Controller"},
	{0xC, "Program Change"},
	{0xD, "Channel Aftertouch"},
	{0xE, "Pitch Bend"},
};
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


uint64_t parse_variable_length(std::ifstream& file) {
	uint64_t value = 0;
	char fetchedOrigValue = 0;
	uint8_t bit = 0;
	
	while(true) {
		bit++;
		assert(bit <= 8 && "Retrieving VLV failed, value longer than 8 Bytes.\n");
		file.read(&fetchedOrigValue, sizeof(char));

		// Append the byte into our variable
		value <<= 7;
		value |= fetchedOrigValue;
		
		if(fetchedOrigValue < 0) {
			break;
		}
	}

	return value;
}


namespace MIDI {
	bool Read(char* file_path) {
		std::ifstream midi_file(file_path, std::ifstream::binary);
		if(!midi_file.good()){
			return false;
		}

		//////////////////////////////////////////
		////         GETTING FILE NAME        ////
		//////////////////////////////////////////
		
		printf("Parsing: %s\n", file_path);
		
		uint16_t     offset   = 0;
		const size_t path_len = strlen(file_path);
		while(offset < path_len) {
			const char symbol = file_path[path_len-offset];
			if(symbol == '/') {
				offset--;
				break;
			}
			offset++;
		}

		char* found_file_name = file_path+(path_len-offset);
		
		printf("File name: %s\n", found_file_name);

		////////////////////////
		
		MIDI_ParsedData midi_header;
		parsed_midi.name = found_file_name;
		
		//////////////////////////////////////////
		////            HEADER PART           ////
		//////////////////////////////////////////
	
		{
			// Checking if it's a MIDI file
			midi_file.read(midi_header.chunk_ID, sizeof(char) * 4);
			midi_header.chunk_ID[4] = '\0';
			if(strcmp(midi_header.chunk_ID, "MThd") != 0) {
				printf("The file is not a midi.\n");
				return false;
			}
			printf("Header ID: %s\n", midi_header.chunk_ID);
		
			//////////////////////////////////////////

			// Get header chunk size
			midi_file.read(reinterpret_cast<char*>(&midi_header.header_size), sizeof(char) * 4);
			flip_bytes_inplace(midi_header.header_size);
			printf("Header size: %u\n", midi_header.header_size);
			
			//////////////////////////////////////////

			// Get Type
			midi_file.read(reinterpret_cast<char*>(&midi_header.type), sizeof(char) * 2);
			flip_bytes_inplace(midi_header.type);
			printf("Type: %d\n", midi_header.type);

			//////////////////////////////////////////

			// Get num of tracks
			midi_file.read(reinterpret_cast<char*>(&midi_header.number_of_tracks), sizeof(char) * 2);
			flip_bytes_inplace(midi_header.number_of_tracks);
			printf("Number of tracks: %d\n", midi_header.number_of_tracks);

			//////////////////////////////////////////

			// Get time division
			// TODO: Translate data into usable thing
			midi_file.read(reinterpret_cast<char*>(&midi_header.time_division), sizeof(char) * 2);
			flip_bytes_inplace(midi_header.time_division);
			printf("Time division: %d\n", midi_header.time_division);
		}

		//////////////////////////////////////////
		////            TRACKS PART           ////
		//////////////////////////////////////////

		printf("\n=======================\nTracks\n=======================\n\n");
		

		parsed_midi.name = found_file_name;

		char     track_chunk_ID[5];
		uint64_t track_chunk_size;
		uint64_t delta_time;
		uint8_t  event;
		for(size_t track_n = 0; track_n < midi_header.number_of_tracks; ++track_n) {
			Track new_track;
			
			// Reading track's header
			midi_file.read(track_chunk_ID, sizeof(char) * 4);
			track_chunk_ID[4] = '\0';

			if(strcmp(track_chunk_ID, "MTrk") != 0) {
				assert(false && "There is no track or MIDI is corrupted.\n");
				return false;
 			}

			// Get chunk size
			midi_file.read(reinterpret_cast<char*>(&track_chunk_size), sizeof(char) * 4);
			flip_bytes_inplace(track_chunk_size);
			printf("Track chunk size: %llu\n", track_chunk_size);
			
			bool is_reading_track = true;
			while(is_reading_track) {
				delta_time = parse_variable_length(midi_file);
				printf("Found vlv: %llu", delta_time);
				
				midi_file.read(reinterpret_cast<char*>(&event), sizeof(char));

				
				if(event == 0xFF) { // Meta events
					MetaEvent new_meta_event;
					char meta_event_type;
					midi_file.read(&meta_event_type, sizeof(char));
					
					
					new_meta_event.type = meta_event_type;
					new_meta_event.name = meta_event_names[meta_event_type];
					switch(meta_event_type) {
						case 0x00: { // Sequence Number
							uint8_t MSB = 0;
							uint8_t LSB = 0;
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
						case 0x07: {	// Cue Point
							const uint64_t meta_event_size = parse_variable_length(midi_file);
							char* text = new char[meta_event_size];
							midi_file.read(text, sizeof(char) * meta_event_size);

							new_meta_event.text = text;
							
							delete[] text;
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						case 0x20: { // MIDI Channel Prefix
							uint8_t channel;
							midi_file.read(reinterpret_cast<char*>(&channel), sizeof(char));
							
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						case 0x2F: { // END OF TRACK
							is_reading_track = false;
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						//    EVENT   |   type    | len | Microseconds/Quarter-Note
						// 255 (0xFF) | 81 (0x51) |  3  | 0-8355711
						// MICROSECONDS_PER_MINUTE = 60000000 BPM = MICROSECONDS_PER_MINUTE / MPQNMPQN = MICROSECONDS_PER_MINUTE / BPM
						// If not provided, the tempo should be set to 120 BPM.
						case 0x51: { // Set Tempo
							uint32_t tempo;
							midi_file.read(reinterpret_cast<char*>(&tempo), sizeof(char) * 3);
							flip_bytes_inplace(tempo);
							
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						case 0x54: { // SMPTE Offset
							uint8_t hour;
							uint8_t min;
							uint8_t sec;
							uint8_t fr;
							uint8_t sub_fr;
							
							midi_file.read(reinterpret_cast<char*>(&hour),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&min),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&sec),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&fr),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&sub_fr),sizeof(char));
							
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						case 0x58: { // Time Signature
							uint8_t number;
							uint8_t denominator;
							uint8_t metro;
							uint8_t _32nds;
							
							midi_file.read(reinterpret_cast<char*>(&number),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&denominator),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&metro),			sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&_32nds),		sizeof(char));
							
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						case 0x59: { // Key Signature
							int8_t  key;	//[-7, 7]
							bool	scale;	//major/minor

							midi_file.read(reinterpret_cast<char*>(&key),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&scale),		sizeof(char));
							
							break;
						}
						case 0x7F: { // Sequencer Specific
							const uint64_t meta_event_size = parse_variable_length(midi_file);
							char* text = new char[meta_event_size];
							midi_file.read(text, sizeof(char) * meta_event_size);
							
							break;
						}
						////////////////////////////////////////////////////////////////////////////////////////////////
						default: {
							printf("This Meta Event is undefined: %x", meta_event_type);
						} 
					}
				}
				else // Non meta events
				{
					Event new_event;
					new_event.type = event;
					new_event.name = meta_event_names[event];
					switch(event) {
						case 0x8: { // Note OFF
							uint8_t channel;
							uint8_t note;
							uint8_t vel;
							
							midi_file.read(reinterpret_cast<char*>(&channel),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&note),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&vel),		sizeof(char));

							break;
						}
						case 0x9: { // Note ON
							uint8_t channel;
							uint8_t note;
							uint8_t vel;
							
							midi_file.read(reinterpret_cast<char*>(&channel),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&note),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&vel),		sizeof(char));

							break;
						}
						case 0xA: { // Note Aftertouch
							uint8_t channel;
							uint8_t note;
							uint8_t amount;
							
							midi_file.read(reinterpret_cast<char*>(&channel),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&note),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&amount),	sizeof(char));

							break;
						}
						case 0xB: { // Controller
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
							uint8_t channel;
							uint8_t controller_type;
							uint8_t value;
							
							midi_file.read(reinterpret_cast<char*>(&channel),			sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&controller_type),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&value),				sizeof(char));

							break;
						}
						case 0xC: { // Program Change 
							uint8_t channel;
							uint8_t program_number;
							
							midi_file.read(reinterpret_cast<char*>(&channel),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&program_number),sizeof(char));

							break;
						}
						case 0xD: { // Channel Aftertouch 
							uint8_t channel;
							uint8_t amount;
							
							midi_file.read(reinterpret_cast<char*>(&channel),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&amount),	sizeof(char));
							
							break;
						}
						case 0xE: { // Pitch Bend
							uint8_t channel;
							uint8_t LSB;
							uint8_t MSB;
							
							midi_file.read(reinterpret_cast<char*>(&channel),	sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&LSB),		sizeof(char));
							midi_file.read(reinterpret_cast<char*>(&MSB),		sizeof(char));
							
							break;
						}
						default: {
							printf("This Event is undefined: %x", event);
						} 
					}
				}
			}

			parsed_midi.tracks.emplace_back(new_track);
		}
		
		
		
		return true;
	}
}
