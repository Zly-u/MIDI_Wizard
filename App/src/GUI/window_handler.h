#pragma once

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL.h>



class window_handler {
public:
	// SDL
	SDL_Renderer*	SDL_Renderer_ptr= nullptr;
	SDL_Window*		SDL_Window_ptr	= nullptr;

	// ImGui
	ImGuiContext* context  = nullptr;
	ImGuiIO*      ImIO_ptr = nullptr;
	
public:
	window_handler();
	
	int  WindowSetup();
	void Cleanup();
};
