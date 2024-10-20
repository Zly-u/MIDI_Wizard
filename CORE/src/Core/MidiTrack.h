#pragma once

#include <memory>

#include "midi_datatypes.h"


class MidiTrack {
public:
	MidiTrack(
	    const std::weak_ptr<Track>& m_data_midi_track,
	    const uint8_t track_id
	) :
        m_data_midi_track(m_data_midi_track),
	    track_id(track_id)
	{}


protected:
	std::shared_ptr<Track> m_data_midi_track;
	uint8_t track_id;
};
