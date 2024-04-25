#include "Object.h"

void Object::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void Object::Update(float dt) {
	IncrimentTime(dt);
}

void Object::SetPos(const float inX, const float inY) {
	rect.x = (int)inX;
	rect.y = (int)inY;
}
