#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "viewport.h"

#include <cassert>

#include "SDL_image.h"

#include "UI_Element_midi_track.h"
#include "Core/MidiParser.h"
#include "ObjectManager.h"
#include "libs/utils.h"

#if MTR_ENABLED
	#include "minitrace.h"
#endif


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
	tex_BG_placeholder = IMG_LoadTexture(main_renderer, "res/place_holder.png");
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
	// const std::shared_ptr<Object> note;
	// note->SetRenderer(main_renderer);
}

void viewport::GenerateMIDI() {
	if(MidiParser::parsed_midi.tracks.empty()) { return; }

	const float track_height = (float)height/(float)MidiParser::parsed_midi.tracks.size();
	const float color_step   = 360.f/(float)MidiParser::parsed_midi.tracks.size();
	int8_t      track_index  = 0;

	for(const auto& midi_track : MidiParser::parsed_midi.tracks) {
	    if(!midi_track){ continue; }

		debug::printf("Track: %s\n", midi_track->name.c_str());

		Object& new_track = ObjectManager::Create<UI_Element_midi_track>(
			main_renderer,
			midi_track,
			(float)width, track_height,
			track_index
		);

		const SDL_Color new_color = utils::HSL2RGB(color_step * track_index, 0.8f, 0.7f);
		new_track.SetColor(new_color);

		track_index++;
	}
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::Update(float dt) {
	
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
	// MTR_SCOPE("Viewport", "SDL_Draw");
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }
	
	SDL_Rect rect{0, 0, width, height};
	SDL_RenderCopy(
		main_renderer,
		tex_BG_placeholder,
		nullptr,
		&rect
	);
	
	ObjectManager::Draw();
}

void viewport::SDL_PostDraw() {
	if(!main_renderer){ return; }
	if(!viewport_tex_ptr){ return; }

	SDL_RenderPresent(main_renderer); // Render the backbuffer
	
	SDL_SetRenderTarget(main_renderer, nullptr); // Stop using render target
}

//----------------------------------------------------------------------------------------------------------------------

void viewport::DrawWindow() {
	constexpr ImGuiWindowFlags MIDI_UI_FLAGS = ImGuiWindowFlags_None
		| ImGuiWindowFlags_NoCollapse
	;

	static bool MIDI_UI_OPEN = true;
	if(MIDI_UI_OPEN)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MIDI Viewport", &MIDI_UI_OPEN, MIDI_UI_FLAGS);
		window_parent = ImGui::GetCurrentWindow();
		
		ImGui::Image(
			(ImTextureID)(intptr_t)viewport_tex_ptr,
			ImVec2(
				ImGui::GetCurrentWindow()->Size.x,
				ImGui::GetCurrentWindow()->Size.y - ImGui::GetCurrentWindow()->TitleBarHeight
			)
		);
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
