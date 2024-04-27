#include "MainWindow.h"

#include <cstdio>
#include <cstdlib>         // abort

#include "imgui_internal.h"
#include "SDL_image.h"


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

int MainWindow::WindowSetup_Impl(){
	constexpr Uint32 SDL_INIT_FLAGS =
		SDL_INIT_VIDEO |
		SDL_INIT_TIMER |
		SDL_INIT_EVENTS;
	if (SDL_Init(SDL_INIT_FLAGS) != 0) { // 0 - success
		printf("SDL Init Error: %s\n", SDL_GetError());
		return -1;
	}
	constexpr Uint32 SDL_IMAGE_INIT_FLAGS =
		IMG_INIT_PNG |
		IMG_INIT_JPG;
	if (IMG_Init(SDL_IMAGE_INIT_FLAGS) == 0) { // 0 - fail
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
	const int desktop_width  = DesktopDisplayMode.w * 0.7;
	const int desktop_height = DesktopDisplayMode.h * 0.7;
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

	constexpr Uint32 MAIN_RENDERER_FLAGS = SDL_RENDERER_ACCELERATED;
	SDL_Renderer_ptr = SDL_CreateRenderer(SDL_Window_ptr, -1, MAIN_RENDERER_FLAGS);
	if(!SDL_Renderer_ptr) {
		CleanUp_Impl();
		printf("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImIO_ptr = &ImGui::GetIO();
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
void MainWindow::CleanUp_Impl(){
	if(SDL_Renderer_ptr) {
		SDL_DestroyRenderer(SDL_Renderer_ptr);
	}
	if(SDL_Window_ptr) {
		SDL_DestroyWindow(SDL_Window_ptr);
	}
	SDL_Quit();
}

