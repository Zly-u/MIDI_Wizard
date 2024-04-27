#include "viewport.h"

#include <cassert>

#include "helpers.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "UI_Element_midi_note.h"


viewport::viewport(SDL_Renderer* renderer)
	: width(1280), height(720),
	  main_renderer(renderer)
{
	viewport_tex_ptr = SDL_CreateTexture(
		main_renderer,
		SDL_PIXELFORMAT_BGRA8888,
		SDL_TEXTUREACCESS_TARGET,
		width, height
	);

	// Load some texture into our viewport render target
	tex_placeholder	= IMG_LoadTexture(main_renderer, "res/place_holder.png");
	assert(tex_placeholder != nullptr);

	OnStart();
}

viewport::~viewport() {
	debug::printf("viewport deconstructed\n");
	if (viewport_tex_ptr) {
		SDL_DestroyTexture(viewport_tex_ptr);
	}
	if(tex_placeholder) {
		SDL_DestroyTexture(tex_placeholder);
	}
}


void viewport::OnStart() {
	const std::shared_ptr<Object> note = std::make_shared<UI_Element_midi_note>(50, 50);
	objects.push_back(note);
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::Update(float dt) {
	for(const auto& obj : objects) {
		obj->Update(dt);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::PreDraw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	// We set our render target (our viewport texture)
	// into which we are gonna draw into
	SDL_SetRenderTarget(main_renderer, viewport_tex_ptr);

	SDL_SetRenderDrawColor(main_renderer, 255, 0, 255, 255);
	SDL_RenderClear(main_renderer);
}

void viewport::Draw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	SDL_Rect rect{0, 0, width, height};
	SDL_RenderCopy(
		main_renderer,
		tex_placeholder,
		nullptr,
		&rect
	);
	
	for(const auto& obj : objects) {
		obj->Draw(main_renderer);
	}
}

void viewport::PostDraw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }

	SDL_RenderPresent(main_renderer); // Render the backbuffer
	
	SDL_SetRenderTarget(main_renderer, nullptr); // Stop using render target
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::DrawUI() {
	bool MIDI_UI_OPEN = true;
	constexpr ImGuiWindowFlags MIDI_UI_FLAGS =
		ImGuiWindowFlags_NoCollapse;
	
	if(MIDI_UI_OPEN) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	}
	
	if(ImGui::Begin("MIDI Viewport", &MIDI_UI_OPEN, MIDI_UI_FLAGS))
	{
		//TODO: Viewport renderer
		ImGui::Image(
			viewport_tex_ptr,
			ImVec2(
				ImGui::GetCurrentWindow()->Size.x,
				ImGui::GetCurrentWindow()->Size.y - ImGui::GetCurrentWindow()->TitleBarHeight()
			)
		);
	} ImGui::End(); /* END Window MIDI Viewport */
	
	if(MIDI_UI_OPEN) {
		ImGui::PopStyleVar();
	}
}
