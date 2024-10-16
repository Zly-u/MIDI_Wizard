﻿	#pragma once

#include <memory>

#include "Core/Object.h"
#include "Core/midi_track.h"

struct Track;

class UI_Element_midi_track : public Object {
public:
	// UI_Element_midi_track() :
	// 	Object(0, 32, 100, 32),
	// 	track_id(0)
	// {}
	
	UI_Element_midi_track(
		const std::shared_ptr<midi_track>& track,
		float new_width, float new_height,
		uint8_t new_track_id
	) :
		Object(0, new_height * new_track_id, new_width, new_height), 
		midi_track_data(track)
	{}
	
protected:
	void Update(float dt) override;
	void Draw() override;

private:
	std::shared_ptr<midi_track> midi_track_data;
};
