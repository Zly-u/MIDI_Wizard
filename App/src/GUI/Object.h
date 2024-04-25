#pragma once
#include "SDL_render.h"

class Object {

public:
	Object(float _x, float _y, float w, float h) : rect(_x, _y, w, h) {
	}
	Object() : Object(0, 0, 32, 32) {}
	
	virtual ~Object() = default;

	
public:
	virtual void Draw(SDL_Renderer* renderer);

	virtual void Update(float dt);

	void SetPos(const float inX, const float inY);
	const SDL_Rect* GetShape() { return &rect; }

	void IncrimentTime(float dt) { time += dt; }
	constexpr float GetTime() { return time; }


private:
	float time = 0;

	SDL_Rect rect;
};
