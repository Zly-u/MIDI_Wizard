#include "UI_Element_midi_note.h"

#include <cmath>

void UI_Element_midi_note::Update(float dt) {
	Object::Update(dt);
	
	SetPos(init_x, init_y + sin(GetTime() * 10.f) * 20.f);
}
