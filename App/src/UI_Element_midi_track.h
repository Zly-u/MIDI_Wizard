#pragma once

#include <memory>

#include "UI_Element_midi_note.h"
#include "utils.h"
#include "Core/math.h"
#include "Core/MidiParser.h"
#include "Object.h"
#include "ObjectManager.h"
#include "Core/MidiTrack.h"

class UI_Element_midi_track : public Object {
public:
	UI_Element_midi_track() : Object(nullptr, 0, 32, 100, 32) {}
	
	UI_Element_midi_track(
		SDL_Renderer* new_renderer,
		Track* track,
		const float new_width, const float new_height,
		const uint8_t new_track_id
	) :
		Object(new_renderer, 0, new_height * new_track_id, new_width, new_height),
		midi_track_data(new MidiTrack(track, new_track_id))
	{
		const auto& NotesPitchRange = midi_track_data->NotesPitchRange;
		const auto& notes  = midi_track_data->GetNotes();
		const float note_h = (float)GetShape()->h/(float)(midi_track_data->NotesPitchRange.GetDifference() + 1);

		for(const auto& note : notes) {
			const float norm_x_pos = math::InvLerp<uint64_t>(0, MidiParser::parsed_midi.length, note.start);

			const float posX_on_track = norm_x_pos * float(GetShape()->w);

			const float norm_note_posY = 1.0f-float(note.pitch - NotesPitchRange.Min)/NotesPitchRange.GetDifference();

			const float note_length = math::InvLerp<uint64_t>(0, MidiParser::parsed_midi.length, note.length) * float(GetShape()->w);

			Object& new_note = ObjectManager::Create<UI_Element_midi_note>(
				GetRenderer(),
				posX_on_track,
				float(GetShape()->y + norm_note_posY * (GetShape()->h - note_h)),
				std::max(note_length, 1.f), std::min(std::max(note_h, 1.f), float(GetShape()->h) / 20.f)
			);
			new_note.SetColor(utils::HSL2RGB(0.f, 0.8f, 0.0f));
		}
	}

protected:
	void Update(float dt) override;
	void Draw() override;

private:
	MidiTrack* midi_track_data;
};
