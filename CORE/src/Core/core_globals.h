#pragma once

#include <map>
#include <string>

namespace globals {
	const std::map<uint8_t, std::string> meta_event_names = {
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
	
	const std::map<uint8_t, std::string> event_names = {
		{0x80, "Note OFF"},
		{0x90, "Note ON"},
		{0xA0, "Note Aftertouch"},
		{0xB0, "Controller"},
		{0xC0, "Program Change"},
		{0xD0, "Channel Aftertouch"},
		{0xE0, "Pitch Bend"},
	};

	constexpr char note_names[12][3] = {
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
}