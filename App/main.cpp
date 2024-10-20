#include <cstdio>

#include "MainWindow.h"
#include "gui.h"
#include "ObjectManager.h"
#include "Core/MidiParser.h"
#include "imgui_impl_sdl2.h"
#include "SDL_events.h"


int wmain(int /*argc*/, wchar_t** /*argv*/){
	if(const int ws_result = MainWindow::WindowSetup() != 0){
		return ws_result;
	}
	
	GUI::Init(MainWindow::Get_SDLRenderer());
	
	constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	wchar_t test_mid[] =
	    L"res/Scatman's-World.mid";
	    // L"res/test_type1_MultiTrack_names.mid";
	    // L"res/SEMBELLO.Maniac.mid";
	    // L"res/test_type0_spaced_CH2.MID";
		// L"res/Pi.mid";
	MidiParser::Read(test_mid);
	GUI::UpdateMIDI();
	
    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true,    do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    	SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
        	
        	switch(event.type) {
        		case SDL_QUIT:
        			done = true;
        			break;
		        default:
        			break;
	        }
        	
        	if (event.type == SDL_WINDOWEVENT &&
        		event.window.event == SDL_WINDOWEVENT_CLOSE &&
        		event.window.windowID == SDL_GetWindowID(MainWindow::Get_SDLWindow())
        	){
        		done = true;
        	}
        }

    	GUI::Update();
    	ObjectManager::Update(1.f/60.f);
    	
    	// Start the ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
    	ImGui_ImplSDL2_NewFrame();
    	ImGui::NewFrame();

    	// Draw in frame
    	GUI::Draw();
    	GUI::Render();

    	// End frame
    	SDL_RenderSetScale(
    		MainWindow::Get_SDLRenderer(),
    		MainWindow::Get_ImGuiIO()->DisplayFramebufferScale.x,
    		MainWindow::Get_ImGuiIO()->DisplayFramebufferScale.y
    	);
    	SDL_SetRenderDrawColor(
    		MainWindow::Get_SDLRenderer(),
    		static_cast<Uint8>(clear_color.x * 255),
    		static_cast<Uint8>(clear_color.y * 255),
    		static_cast<Uint8>(clear_color.z * 255),
    		static_cast<Uint8>(clear_color.w * 255)
    	);
    	SDL_RenderClear(MainWindow::Get_SDLRenderer());
    	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), MainWindow::Get_SDLRenderer());

    	// Display all drawn contents from the backbuffer
    	SDL_RenderPresent(MainWindow::Get_SDLRenderer());

    	SDL_Delay(16);
    }

	printf("\nCLEANING UP\n\n");
	
    return 0;
}
