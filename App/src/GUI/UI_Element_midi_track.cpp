#include "UI_Element_midi_track.h"

#include "midi.h"

void UI_Element_midi_track::Update(float dt) {
	Object::Update(dt);
}

// TODO: Drawing notes, prolly need to translate Midi events as more easy to work with data.
void UI_Element_midi_track::Draw() {
	Object::Draw();
	
	// if(m_data_midi_track.expired()) { return; }
	//
	// std::shared_ptr<Track> track = m_data_midi_track.lock();
	// track->events["Note ON"];
}