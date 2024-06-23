#include "viewport.h"

#include <cassert>

#include "helpers.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "midi.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "UI_Element_midi_note.h"
#include "UI_Element_midi_track.h"
#include "libs/utils.h"


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
	tex_BG_placeholder	= IMG_LoadTexture(main_renderer, "res/place_holder.png");
	assert(tex_BG_placeholder != nullptr);
}

viewport::~viewport() {
	debug::printf("viewport deconstructed\n");
	if (viewport_tex_ptr) {
		SDL_DestroyTexture(viewport_tex_ptr);
	}
	if(tex_BG_placeholder) {
		SDL_DestroyTexture(tex_BG_placeholder);
	}
}


void viewport::OnStart() {
	const std::shared_ptr<Object> note = std::make_shared<UI_Element_midi_note>(50, 50);
	note->SetRenderer(main_renderer);
	objects.push_back(note);
}

void viewport::UpdateMIDI() {
	if(MIDI::parsed_midi.tracks.size() == 0) { return; }

	tracks.clear();
	
	const float track_height = height/(MIDI::parsed_midi.tracks.size());
	const float color_step = 360.f/(MIDI::parsed_midi.tracks.size());
	int8_t track_index = 0;
	
	for(std::shared_ptr<Track>& midi_track : MIDI::parsed_midi.tracks) {
		debug::printf("Track: %s\n", midi_track->name.c_str());
		
		const std::shared_ptr<Object> new_track = std::make_shared<UI_Element_midi_track>(
			midi_track, width, track_height, track_index 
		);
		new_track->SetRenderer(main_renderer);

		SDL_Color new_color = utils::HSL2RGB(color_step * track_index, 0.8f, 0.7f);
		new_track->SetColor(new_color);
		
		tracks.push_back(new_track);
		track_index++;
	}
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::Update(float dt) {
	for(const auto& obj : objects) {
		obj->Update(dt);
	}
	for(const auto& track : tracks) {
		track->Update(dt);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::SDL_PreDraw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	// We set our render target (our viewport texture)
	// into which we are gonna draw into
	SDL_SetRenderTarget(main_renderer, viewport_tex_ptr);

	SDL_SetRenderDrawColor(main_renderer, 255, 0, 255, 255);
	SDL_RenderClear(main_renderer);
}

void viewport::SDL_Draw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	SDL_Rect rect{0, 0, width, height};
	SDL_RenderCopy(
		main_renderer,
		tex_BG_placeholder,
		nullptr,
		&rect
	);
	
	for(const auto& obj : objects) {
		obj->Draw();
	}
	for(const auto& track : tracks) {
		track->Draw();
	}
}

void viewport::SDL_PostDraw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }

	SDL_RenderPresent(main_renderer); // Render the backbuffer
	
	SDL_SetRenderTarget(main_renderer, nullptr); // Stop using render target
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::DrawWindow() {
	bool MIDI_UI_OPEN = true;
	constexpr ImGuiWindowFlags MIDI_UI_FLAGS =
		ImGuiWindowFlags_NoCollapse;
	
	if(MIDI_UI_OPEN) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	}
	
	if(ImGui::Begin("MIDI Viewport", &MIDI_UI_OPEN, MIDI_UI_FLAGS))
	{
		
		window_parent = ImGui::GetCurrentWindow();
		
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
