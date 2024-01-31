#pragma once

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL.h>



class window_handler {
public:
	// Data
	SDL_Renderer*	SDL_Renderer_ptr= nullptr;
	ImGuiIO*		ImIO_ptr		= nullptr;
	SDL_Window*		SDL_Window_ptr	= nullptr;
	
public:
	window_handler();
	
	int  WindowSetup();
	void Cleanup();
};
