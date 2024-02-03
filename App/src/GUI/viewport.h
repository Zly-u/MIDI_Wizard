#pragma once
#include <SDL_render.h>


class viewport {
public:
	viewport() = default;
	explicit viewport(SDL_Renderer* renderer);
	~viewport();

	// Made pre and post for ease of coding graphics
	void PreDraw();
	void Draw();
	void PostDraw();

	void DrawUI();
	
////////////////////////////////////////////////////////
	
public:
	int width  = -1;
	int height = -1;

	SDL_Renderer* main_renderer    = nullptr;
	SDL_Texture*  viewport_tex_ptr = nullptr;
	SDL_Texture*  tex_placeholder  = nullptr;
};
