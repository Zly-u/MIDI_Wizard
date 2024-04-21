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


static std::map<uint8_t, std::string> meta_event_names = {
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
static std::map<uint8_t, std::string> event_names = {
	{0x80, "Note OFF"},
	{0x90, "Note ON"},
	{0xA0, "Note Aftertouch"},
	{0xB0, "Controller"},
	{0xC0, "Program Change"},
	{0xD0, "Channel Aftertouch"},
	{0xE0, "Pitch Bend"},
};

static char note_names[][3] = {
	"C ",
	"C#",
	"D ",
	"D#",
	"E ",
	"F ",
	"F#",
	"G ",
	"G#",
	"A ",
	"A#",
	"B ",
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

struct Note {
	uint8_t  tone = 0;
	uint8_t  vel  = 0;
	uint64_t time = 0;
};

struct SysExEvent {
	uint8_t     type = 0;
	uint64_t    time   = 0;
	uint64_t    packet = 0;
	
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
	uint64_t channel = 0;

	uint64_t value1 = 0;
	uint64_t value2 = 0;

	std::string name;
};


struct Track {
	Track() = default;
	
	std::string name;

	uint8_t channel = 0;

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
