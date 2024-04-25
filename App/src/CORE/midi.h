#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "helpers.h"


namespace std {
	class stop_token;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

struct Note {
	uint8_t  tone = 0;
	uint8_t  vel  = 0;
	uint64_t time = 0;
};

struct SysExEvent {
	uint8_t     type	= 0;
	uint64_t    time	= 0;
	uint64_t    packet	= 0;
	
	std::string name;
};


struct MetaEvent {
	uint8_t type = 0;
	uint64_t time = 0;

	uint64_t value1 = 0;
	uint64_t value2 = 0;
	uint64_t value3 = 0;
	uint64_t value4 = 0;
	uint64_t value5 = 0;

	std::string name;
	std::string text;
};

struct MIDI_Event {
	uint8_t  type    = 0;
	uint64_t time    = 0;
	uint8_t	 channel = 0;

	uint64_t value1 = 0;
	uint64_t value2 = 0;

	std::string name;
};


struct Track {
	Track() = default;
	
	std::string name;

	int8_t channel = -1;

	std::map<
		std::string,
		std::vector<std::shared_ptr<MetaEvent>>
	> meta_events;
	
	std::map<
		std::string,
		std::vector<std::shared_ptr<MIDI_Event>>
	> events;
};


struct midi {
	std::string name;
	std::string time_signature = "4/4";
	uint16_t bpm = 0;
	
	std::vector<std::shared_ptr<Track>> tracks;

	~midi() {
		debug::printf("Parsed MIDI destructed\n");
	}
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

namespace MIDI {
	void worker_TrackRead(const std::stop_token& stop_token, char* file_path, uint8_t track_index);
	bool Read(char* file_path);
}
