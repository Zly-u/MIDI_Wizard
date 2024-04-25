#pragma once

class  window_handler;
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

namespace gui
{
	static SDL_Texture* tex_placeholder = nullptr;
	static SDL_Texture* viewport_tex	= nullptr;

	///
	
	void Init(SDL_Renderer* renderer);
	void SetupDocking();
	void Update();
	void DrawUI();
	void Render();
	void Cleanup();

	/// UI ELEMENTS ///
	
	void UI_ShowMenu_File();

	/// UI HELPERS ///

	void HelpMarker(const char* desc);
};

