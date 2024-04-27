#pragma once

#include <memory>
#include <vector>

#include <SDL_render.h>
#include "imgui.h"
#include "UI_Element_midi_note.h"


class viewport {
public:
	viewport() = default;
	explicit viewport(SDL_Renderer* renderer);
	~viewport();
	
	// Made pre and post for ease of coding graphics
	void PreDraw();
	void OnStart();
	void Update(float dt);
	void Draw();
	void PostDraw();

	void DrawUI();
	
////////////////////////////////////////////////////////
	
public:
	std::vector<std::shared_ptr<Object>> objects;
	
	int width  = -1;
	int height = -1;

	ImVec2 current_window_size{-1, -1};

	// NOTE: `main_renderer` - prolly make it assignable by any passed target, so it behaves as an actual projectable viewport.
	SDL_Renderer* main_renderer    = nullptr;
	SDL_Texture*  viewport_tex_ptr = nullptr;
	SDL_Texture*  tex_placeholder  = nullptr;
};
