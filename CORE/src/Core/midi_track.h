#pragma once
#include <memory>

#include "midi.h"

class midi_track {
public:
	midi_track(const std::weak_ptr<Track>& m_data_midi_track, uint8_t track_id)
		: m_data_midi_track(m_data_midi_track),
		  track_id(track_id)
	{}


protected:
	std::weak_ptr<Track> m_data_midi_track;
	uint8_t track_id;
};
