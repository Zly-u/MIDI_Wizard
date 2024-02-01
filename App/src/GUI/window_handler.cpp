#include "window_handler.h"

#include <cstdio>
#include <cstdlib>         // abort

#include "imgui_internal.h"



	
struct WindowSettings
{
	const char* Title = "MIDI Wizard";
	int width = 1280;
	int height = 720;
	const SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI
	);

	ImGuiConfigFlags ConfigFlags =
		ImGuiConfigFlags_NavEnableKeyboard |     // Enable Keyboard Controls
		ImGuiConfigFlags_DockingEnable |         // Enable Docking
		ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	// | ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	// | ImGuiConfigFlags_ViewportsNoMerge;
};


window_handler::window_handler() :
	ImIO_ptr(nullptr),
	SDL_Window_ptr(nullptr)
{}



int window_handler::WindowSetup(){
	// Setup SDL
	constexpr Uint32 SDL_INIT_FLAGS =
		SDL_INIT_VIDEO |
		SDL_INIT_TIMER |
		SDL_INIT_EVENTS;
	if (SDL_Init(SDL_INIT_FLAGS) != 0) {
		printf("SDL Init Error: %s\n", SDL_GetError());
		return -1;
	}

	// From 2.0.18: Enable native IME.
	#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	#endif

	// Create window with SDL_Renderer graphics context
	constexpr SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI |
		SDL_WINDOW_SHOWN
	);
	SDL_DisplayMode DesktopDisplayMode;
	SDL_GetDesktopDisplayMode(0, &DesktopDisplayMode);
	const int desktop_width = DesktopDisplayMode.w * 0.9;
	const int desktop_height = DesktopDisplayMode.h * 0.9;
	SDL_Window_ptr = SDL_CreateWindow(
		"MIDI Wizard",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		desktop_width, desktop_height,
		window_flags
	);
	if (!SDL_Window_ptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer_ptr = SDL_CreateRenderer(SDL_Window_ptr, -1, 0);
	if(!SDL_Renderer_ptr) {
		Cleanup();
		printf("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImIO_ptr = &ImGui::GetIO(); (void)ImIO_ptr;
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForSDLRenderer(
		SDL_Window_ptr,
		SDL_Renderer_ptr
	);
	ImGui_ImplSDLRenderer2_Init(SDL_Renderer_ptr);

	return 0;
}

// TODO: texture destroying
void window_handler::Cleanup(){
	if(SDL_Renderer_ptr) {
		SDL_DestroyRenderer(SDL_Renderer_ptr);
	}
	if(SDL_Window_ptr) {
		SDL_DestroyWindow(SDL_Window_ptr);
	}
	SDL_Quit();
}

