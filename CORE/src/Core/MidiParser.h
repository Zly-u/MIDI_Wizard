#pragma once

#include <cstdint>
#include <mutex>

#include "midi_datatypes.h"


namespace std {
	class stop_token;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

class MidiParser {
public:
	static MIDI_ParsedData midi_header;
	static midi parsed_midi;
	
	static void worker_TrackRead(const std::stop_token& stop_token, const wchar_t* file_path, const uint16_t track_index);
	static bool Read(wchar_t* file_path);

private:
	static std::mutex g_track_write_mutex;
};
