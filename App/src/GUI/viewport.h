#pragma once

#include <memory>
#include <vector>

#include <SDL_render.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "UI_Element_midi_note.h"


struct ImGuiWindow;

class viewport {
public:
	viewport() = default;
	explicit viewport(SDL_Renderer* renderer);
	~viewport();
	
	// Made pre and post for ease of coding graphics
	void SDL_PreDraw();
	void OnStart();
	void UpdateMIDI();
	void Update(float dt);
	void SDL_Draw();
	void SDL_PostDraw();

	void DrawWindow();

	////////////////////

	[[nodiscard]] const ImGuiWindow* GetWindow() const { return window_parent; }

	void SetCurrentWindow(ImGuiWindow* _win) {
		window_parent = _win;
		current_window_size = ImVec2(
			window_parent->Size.x,
			window_parent->Size.y - ImGui::GetCurrentWindow()->TitleBarHeight()
		);
	}
	
////////////////////////////////////////////////////////
	
public:
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<Object>> tracks;
	
	int width  = -1;
	int height = -1;

	ImVec2 current_window_size{-1, -1};
	
	ImVec2 zoom{1.0f, 1.0f};
	ImVec2 scroll{0.0f, 0.0f};

private:
	// NOTE: `main_renderer` - prolly make it assignable by any passed target, so it behaves as an actual projectable viewport.
	SDL_Renderer* main_renderer    = nullptr;
	SDL_Texture*  viewport_tex_ptr = nullptr;
	SDL_Texture*  tex_BG_placeholder  = nullptr;

	ImGuiWindow* window_parent = nullptr;
};
