#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>


struct Note {
	uint64_t delta = 0;
	uint8_t  tone  = 0;
	uint8_t  vel   = 0;
};


struct MetaEvent {
	uint8_t type;
	std::string name;
	
	std::string text;
};

struct Event {
	uint8_t type;
	std::string name;
	
	std::string text;

	
};


struct Track {
	std::string name;

	uint8_t channel = 0;

	std::vector<Note> notes;
	
	std::map<
		std::string,
		std::vector<Event>
	> events;
};


static struct midi {
	std::string name;
	std::string time_signature = "4/4";
	uint16_t bpm = 0;
	
	std::vector<Track> tracks;
} parsed_midi;


namespace MIDI {
	bool Read(char* file_path);
}
