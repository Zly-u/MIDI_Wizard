#pragma once
#include <SDL_render.h>


class viewport {
public:
	SDL_Texture*  viewport_tex_ptr	= nullptr;
	SDL_Renderer* main_renderer	= nullptr;
	SDL_Texture* tex_placeholder = nullptr;

public:
	viewport() = default;
	explicit viewport(SDL_Renderer* renderer);
	~viewport();
	
	void Draw();
	void Delete();
};
