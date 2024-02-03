#include "midi.h"

#include <fstream>

////////////////////////////////////////////////////////////////////////

struct MIDI_ParsedData {
	char           chunk_ID[5];
	uint32_t       header_size      = 0;
	unsigned short type             = 0; // 0-2
	unsigned short number_of_tracks = 0; // 1-65535
	unsigned short time_division    = 0;
};

struct EventsData {
	char* channel;
	char* system_exclusive;
	char* meta;
};

struct TrackParsedData {
	char     chunk_ID[5];
	uint32_t chunk_size = 0;

	EventsData events;
};


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

		const std::string path            = file_path;
		const std::string found_file_name = path.substr(path_len - offset, offset);
		
		if(found_file_name.empty()) {
			printf("Couldn't parse the path: %s\n", file_path);
			return false;
		}
		
		printf("File name: %s\n", found_file_name.c_str());

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

			// Get chunk size
			midi_file.read(reinterpret_cast<char*>(&midi_header.header_size), sizeof(char) * 4);
			midi_header.header_size = _byteswap_ulong(midi_header.header_size);
			printf("Header size: %u\n", midi_header.header_size);

			//////////////////////////////////////////

			// Get Type
			midi_file.read(reinterpret_cast<char*>(&midi_header.type), sizeof(char) * 2);
			midi_header.type = _byteswap_ushort(midi_header.type);
			printf("Type: %d\n", midi_header.type);

			//////////////////////////////////////////

			// Get num of tracks
			midi_file.read(reinterpret_cast<char*>(&midi_header.number_of_tracks), sizeof(char) * 2);
			midi_header.number_of_tracks = _byteswap_ushort(midi_header.number_of_tracks);
			printf("Number of tracks: %d\n", midi_header.number_of_tracks);

			//////////////////////////////////////////

			// Get time division
			// TODO: Translate data into usable thing
			midi_file.read(reinterpret_cast<char*>(&midi_header.time_division), sizeof(char) * 2);
			midi_header.time_division = _byteswap_ushort(midi_header.time_division);
			printf("Time division: %d\n", midi_header.time_division);
		}

		//////////////////////////////////////////
		////            TRACKS PART           ////
		//////////////////////////////////////////

		printf("\n=======================\nTracks\n=======================\n\n");
		
		TrackParsedData track_chunk;
		
		// Reading track's header
		midi_file.read(track_chunk.chunk_ID, sizeof(char) * 4);
		track_chunk.chunk_ID[4] = '\0';

		if(strcmp(track_chunk.chunk_ID, "MTrk") != 0) {
			printf("There is no track.\n");
			return false;
		}

		// Get chunk size
		midi_file.read(reinterpret_cast<char*>(&midi_header.header_size), sizeof(char) * 4);
		track_chunk.chunk_size = _byteswap_ulong(midi_header.header_size);
		printf("Track chunk size: %u\n", track_chunk.chunk_size);

		
		
		return true;
	}
}
