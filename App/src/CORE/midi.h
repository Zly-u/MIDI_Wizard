#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>


struct Note {
	uint64_t time = 0;
	uint8_t  tone = 0;
	uint8_t  vel  = 0;
};


struct MetaEvent {
	uint8_t     type;
	std::string name;

	uint64_t	time = 0;

	uint64_t	value1 = 0;
	uint64_t	value2 = 0;
	uint64_t	value3 = 0;
	uint64_t	value4 = 0;
	uint64_t	value5 = 0;
	
	std::string text;
};

struct Event {
	uint8_t     type;
	std::string name;

	uint64_t	time = 0;

	uint64_t	value1 = 0;
	uint64_t	value2 = 0;
	uint64_t	value3 = 0;
	uint64_t	value4 = 0;
	uint64_t	value5 = 0;
	
	std::string text;
};


struct Track {
	std::string name;

	uint8_t channel = 0;

	std::map<
		std::string,
		std::vector<MetaEvent>
	> meta_events;
	
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
