#include "viewport.h"

#include <cstdio>

#include "SDL_render.h"


viewport::viewport(::SDL_Renderer* renderer) /*: main_renderer(renderer)*/{
	viewport_tex_ptr = SDL_CreateTexture(
		main_renderer,
		SDL_PIXELFORMAT_BGRA8888,
		SDL_TEXTUREACCESS_TARGET,
		1280, 720
	);
}

viewport::~viewport() {
	printf("viewport deconstructed\n");
	if (viewport_tex_ptr) {
		SDL_DestroyTexture(viewport_tex_ptr);
	}
}


void viewport::Draw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	// We set our render target (our viewport texture)
	// into which we are gonna draw into
	SDL_SetRenderTarget(main_renderer, viewport_tex_ptr);
	{
		SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
		SDL_RenderClear(main_renderer);
	}
	SDL_SetRenderTarget(main_renderer, nullptr); // Stop using render target
}

void viewport::Delete() {
	if(viewport_tex_ptr) {
		SDL_DestroyTexture(viewport_tex_ptr);
	}
}
