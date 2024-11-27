#pragma once

#include <map>
#include <vector>
#include <string>

#include "helpers.h"

#pragma pack(push, 1)
struct Note {
    uint8_t  tone = 0;
    uint8_t  vel  = 0;
    uint64_t time = 0;
};


struct SysExEvent {
    uint8_t  type   = 0;
    uint64_t time   = 0;
    uint64_t packet = 0;

    std::string name;
};


struct MetaEvent {
    uint8_t  type = 0;
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
    uint8_t  channel = 0;
    uint64_t time    = 0;

    uint64_t value1 = 0;
    uint64_t value2 = 0;

    std::string name;
};

struct MIDI_ParsedData {
	uint32_t header_size       = 0;
	uint16_t  type             = 0; // 0-2
	uint16_t  number_of_tracks = 1; // 1-65535

	// TODO: time_division
	uint8_t time_division   = 0;
	uint8_t ticks_per_qn    = 0;
	int8_t  SMPTE_format    = 0;
	uint8_t ticks_per_frame = 0;
};
#pragma pack(pop)

struct Track {
	Track() {
		debug::printf("Track Created\n");
	}
    ~Track() {
        debug::printf("~Track(%s)\n", name.c_str());
    	meta_events.clear();
    	events.clear();
    }

    std::string name = "[PLACEHOLDER NAME]";

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
	std::wstring name;
	std::string time_signature = "4/4";
	uint16_t bpm = 0;
	uint64_t length = 0;

	std::vector<std::shared_ptr<Track>> tracks;

	~midi() {
		debug::printf("Parsed MIDI destructed\n");
		tracks.clear();
	}
};