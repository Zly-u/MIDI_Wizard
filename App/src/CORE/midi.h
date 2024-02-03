#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>


// TODO: Notes
struct note {
	uint8_t tone = 0;
	float   pan  = 0.0f;
	float   vel  = 0.0f;
};

// TODO: Tracks
struct track {
	std::vector<uint8_t>           notes;
	std::map<std::string, uint8_t> cc;
};

// TODO: Lyrics
struct lyrics_text {
	std::string text;
	float       length;
};

// TODO: Midi
static struct midi {
	std::string name;
	std::string time_signature = "4/4";
	uint16_t bpm = 0;
	
	std::vector<track>       tracks;
	std::vector<lyrics_text> lyrics; //NOTE: Prob use map for using keys as time starts
} parsed_midi;


namespace MIDI {
	bool Read(char* file_path);
}
