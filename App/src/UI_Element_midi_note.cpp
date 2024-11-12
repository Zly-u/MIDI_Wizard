#include "UI_Element_midi_note.h"

#include <cmath>

void UI_Element_midi_note::Update(float dt) {
	Object::Update(dt);
	
	//SetPos(init_x, init_y + sin(init_x/10.f + GetTime() * 10.f) * 10.f);
}
