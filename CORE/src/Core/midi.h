#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
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
	~Track() {
		debug::printf("~Track(%s)", name.c_str());
	}
	
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


struct MIDI_ParsedData {
	uint32_t       header_size      = 0;
	unsigned short type             = 0; // 0-2
	unsigned short number_of_tracks = 1; // 1-65535
	
	// TODO: time_division
	unsigned short time_division   = 0;
	unsigned short ticks_per_qn    = 0;
	char           SMPTE_format    = 0;
	unsigned short ticks_per_frame = 0;
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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

class MIDI {
public:
	static midi parsed_midi;
	
	static void worker_TrackRead(const std::stop_token& stop_token, char* file_path, uint8_t track_index);
	static bool Read(char* file_path);

private:
	static std::mutex g_track_write_mutex;
};
