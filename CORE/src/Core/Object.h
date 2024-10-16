#pragma once

#include "imgui.h"
#include "SDL_render.h"


class Object {

public:
	Object(){};
	
	Object(float _x, float _y, float w, float h) :
		m_rect((int)_x, (int)_y, (int)w, (int)h),
		m_init_rect(m_rect)
	{}
	
	virtual ~Object() = default;

public:
	// Updates User Input
	virtual void UpdateEvents();
	virtual void Update(float dt);
	virtual void Draw();
	
public:
	void SetPos(const float inX, const float inY);

	void SetRenderer(SDL_Renderer* renderer) {
		m_renderer = renderer;
	}
	constexpr SDL_Renderer* GetRenderer() {
		return m_renderer;
	}

	void SetColor(SDL_Color new_color) {
		m_color = new_color;
	}
	const SDL_Color& GetColor() {
		return m_color;
	}
	
	void SetSizeRef(ImVec2* SizeRef){ size_mult = SizeRef; }
	
	constexpr SDL_Rect* GetShape() { return &m_rect; }
	
	constexpr float GetTime() { return m_time; }

	////////////////////////////////////////////////////////////

	// Prolly was related to Viewport inverse scaling
	ImVec2 GetSizeMult() {
		return size_mult ? *size_mult : ImVec2{1.0f, 1.0f};
	}
	
	
protected:
	float m_time = 0;

	SDL_Renderer* m_renderer;
	SDL_Rect m_rect;
	SDL_Rect m_init_rect;
	SDL_Color m_color{0, 150, 250, 255};

	ImVec2* size_mult = nullptr;
};
