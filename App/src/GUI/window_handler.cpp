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
		SDL_INIT_AUDIO |
		SDL_INIT_EVENTS;
	if (SDL_Init(SDL_INIT_FLAGS) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// From 2.0.18: Enable native IME.
	#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	#endif

	// Create window with Vulkan graphics context
	constexpr SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		// SDL_WINDOW_VULKAN |
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI |
		SDL_WINDOW_SHOWN
	);
	SDL_Window_ptr = SDL_CreateWindow(
		"MIDI Wizard",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		window_flags
	);
	if (!SDL_Window_ptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer_ptr = SDL_CreateRenderer(SDL_Window_ptr, -1, 0);
	if(!SDL_Renderer_ptr) {
		Cleanup();
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGuiContext* CTX = ImGui::CreateContext();
	ImIO_ptr = &CTX->IO;
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImIO_ptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	
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

