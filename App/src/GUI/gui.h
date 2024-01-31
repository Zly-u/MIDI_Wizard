#pragma once

#include <map>
#include <SDL_render.h>
#include <string>

#include "imgui.h"

class window_handler;

// struct TextureData {
// 	SDL_Surface* tex = nullptr;
// 	
// 	int width	= -1;
// 	int height	= -1;
// };

namespace res {
	// ImTextureID* tex_placeHolder = nullptr;
	// std::map<std::string, TextureData> tex = {
	// 	{"place_holder", TextureData()}
	// };
}

namespace gui
{
	static SDL_Texture* viewport_tex = nullptr;
	
	void Init(struct SDL_Renderer* renderer);
	void DrawUI();
	void Render();
	void Cleanup();
};

