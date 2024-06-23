#pragma once

#include <memory>

#include "Object.h"
#include "viewport.h"

struct Track;

class UI_Element_midi_track : public Object {
public:
	UI_Element_midi_track(
		const std::shared_ptr<Track>& track,
		float new_width, float new_height,
		uint8_t new_track_id
	) :
		Object(0, new_height * new_track_id, new_width, new_height), 
		m_data_midi_track(track),
		track_id(new_track_id)
	{}
	
protected:
	void Update(float dt) override;
	void Draw() override;

private:
	std::weak_ptr<Track> m_data_midi_track;
	uint8_t track_id;
};
