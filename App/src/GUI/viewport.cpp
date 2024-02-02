#include "viewport.h"

#include <cassert>
#include <cstdio>

#include "SDL_image.h"
#include "SDL_render.h"


viewport::viewport(SDL_Renderer* renderer)
	: main_renderer(renderer)
{
	viewport_tex_ptr = SDL_CreateTexture(
		main_renderer,
		SDL_PIXELFORMAT_BGRA8888,
		SDL_TEXTUREACCESS_TARGET,
		1280, 720
	);

	// Load some texture into our viewport render target
	tex_placeholder	= IMG_LoadTexture(main_renderer, "res/place_holder.png");
	assert(tex_placeholder != nullptr);
}

viewport::~viewport() {
	printf("viewport deconstructed\n");
	if (viewport_tex_ptr) {
		SDL_DestroyTexture(viewport_tex_ptr);
	}
	if(tex_placeholder) {
		SDL_DestroyTexture(tex_placeholder);
	}
}


void viewport::Draw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	// We set our render target (our viewport texture)
	// into which we are gonna draw into
	SDL_SetRenderTarget(main_renderer, viewport_tex_ptr);
	{
		SDL_RenderClear(main_renderer);
		
		SDL_SetRenderDrawColor(main_renderer, 255, 0, 255, 255);
		SDL_RenderPresent(main_renderer);
		
		SDL_Rect rect{50, 50, 100, 100,};
		SDL_RenderCopy(
			main_renderer,
			tex_placeholder,
			nullptr,
			&rect
		);
		SDL_RenderPresent(main_renderer);

		
	}
	SDL_SetRenderTarget(main_renderer, nullptr); // Stop using render target
}

void viewport::Delete() {
	viewport::~viewport();
}
