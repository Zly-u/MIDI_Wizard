#pragma once
#include <memory>

#include "imgui.h"

#include "viewport.h"

struct SDL_Texture;
struct SDL_Renderer;

// struct TextureData {
// 	SDL_Surface* tex = nullptr;
// 	
// 	int width	= -1;
// 	int height	= -1;
// };

namespace res {
	// ImTextureID* tex_placeHolder = nullptr;
	// std::map<std::string, TextureData> tex = {
	// 	{"place_holder", TextureData()}
	// };
	
}

// GUI Singleton
class GUI
{
public:
	GUI(const GUI&) = delete;
	~GUI() {
		CleanUp_Impl();
	}
	
	static GUI& Get() {
		static GUI s_instance;
		return s_instance;
	}

	///
	
	static void Init(SDL_Renderer* renderer) {
		Get().Init_Impl(renderer);
	}
	static void UpdateMIDI() {
		Get().UpdateMIDI_Impl();
	}
	static void SetupDocking(){
		Get().SetupDocking_Impl();
	}
	static void Update(){
		Get().Update_Impl();
	}
	static void Draw(){
		Get().Draw_Impl();
	}
	static void Render(){
		Get().Render_Impl();
	}
	static void CleanUp(){
		Get().CleanUp_Impl();
	}

	/// UI ELEMENTS ///
	
	static void UI_ShowMenu_File() {
		Get().UI_ShowMenu_File_Impl();
	}
	static void HelpMarker(const char* desc) {
		Get().HelpMarker_Impl(desc);
	}

	
private:
	GUI(){}
	
	void Init_Impl(SDL_Renderer* renderer);
	void UpdateMIDI_Impl();
	void SetupDocking_Impl();
	void Update_Impl();
	void Draw_Impl();
	void Render_Impl();
	void CleanUp_Impl();
	
	/// UI ELEMENTS ///

	void UI_ShowMenu_File_Impl();
	void HelpMarker_Impl(const char* desc);

	
private:
	bool dock_is_open	= true;
	bool opt_fullscreen	= true;
	bool opt_padding	= false; // Removes padding for all the child windows

	// Midi Channels Window
	bool MIDI_UI_CHANNELS_OPEN = true;
	const ImGuiWindowFlags MidiChannels_UI_flags = ImGuiWindowFlags_None;
	
	// Our state
	bool   show_demo_window    = true;
	bool   show_another_window = false;
	ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	SDL_Texture* tex_placeholder	= nullptr;
	SDL_Texture* viewport_tex		= nullptr;

	std::unique_ptr<viewport> midi_viewport;
};

