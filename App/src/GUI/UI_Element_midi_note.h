#pragma once

#include "Object.h"

class UI_Element_midi_note : public Object {
public:
	UI_Element_midi_note() : Object(0, 0, 32*5, 32), init_x(0), init_y(0) {}
	UI_Element_midi_note(float _x, float _y, float w = 32*5, float h = 32) : Object(_x, _y, w, h), init_x(_x), init_y(_y) {}
	
protected:
	void Update(float dt) override;

private:
	float init_x;
	float init_y;
};
