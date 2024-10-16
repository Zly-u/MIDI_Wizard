#pragma once

#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"

#include "SDL.h"

// Window Singleton
class MainWindow {
	
public:
	MainWindow(const MainWindow&) = delete; 

	
public:
	~MainWindow() {
		CleanUp_Impl();
	}
	
	static MainWindow& Get() {
		static MainWindow s_instance;
		return s_instance;
	}

	static int WindowSetup() {
		return Get().WindowSetup_Impl();
	}

	static void CleanUp() {
    	return Get().CleanUp_Impl();
    }

	// SDL Getters
	
	static SDL_Renderer* Get_SDLRenderer() {
		return Get().SDL_Renderer_ptr;
	}
	static SDL_Window* Get_SDLWindow() {
		return Get().SDL_Window_ptr;
	}

	// ImGUI Getters
	
	static ImGuiContext* Get_ImGuiContext() {
		return Get().context;
	}
	
	static ImGuiIO* Get_ImGuiIO() {
		return Get().ImIO_ptr;
	}
	
private:
	MainWindow() :
		SDL_Window_ptr(nullptr),
		ImIO_ptr(nullptr)
	{}
	
	int  WindowSetup_Impl();
	void CleanUp_Impl();


private:
	// SDL
	SDL_Renderer*	SDL_Renderer_ptr= nullptr;
	SDL_Window*		SDL_Window_ptr	= nullptr;

	// ImGui
	ImGuiContext* context  = nullptr;
	ImGuiIO*      ImIO_ptr = nullptr;
};
