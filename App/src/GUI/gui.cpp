#include "gui.h"

#include <memory>
#include <SDL_render.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_internal.h" 
#include "SDL_image.h"
#include "viewport.h"



namespace gui {
	// Our state
	bool show_demo_window		= true;
	bool show_another_window	= false;
	ImVec4 clear_color			= ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	std::unique_ptr<viewport> midi_viewport;
	
	void Init(SDL_Renderer* renderer) {
		midi_viewport = std::make_unique<viewport>(renderer);

		// ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();
		
		// Load some texture into our viewport render target
		// SDL_Surface* surf = IMG_Load("E:/JetBrains_Rider/Projects_CPP/MIDI_Wizard/App/res/place_holder.png");
		// tex_placeholder = SDL_CreateTextureFromSurface(renderer, surf);
		// SDL_FreeSurface(surf);

		// constexpr SDL_Rect rect{0, 0, 100, 100,};
		// SDL_RenderCopy(renderer, tex, nullptr, &rect);
	}

	
	void DrawUI()
	{
		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				UI_ShowMenu_File();
				ImGui::EndMenu(); // File
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}

				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item

				ImGui::Separator();

				if (ImGui::MenuItem("Cut", "CTRL+X")) {}

				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				
				ImGui::EndMenu(); // Edit
			}
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Scale")) {}

				ImGui::Separator();
				
				if (ImGui::BeginMenu("Themes"))
				{
					if(ImGui::MenuItem("Dark")) {
						ImGui::StyleColorsDark();
					}
					if(ImGui::MenuItem("Light")){
						ImGui::StyleColorsLight();
					}
					ImGui::EndMenu();
				}
				
				ImGui::EndMenu(); // Options
			}
			ImGui::EndMainMenuBar();
		}

		/*
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;
		
			// Create a window called "Hello, world!" and append into it.
			ImGui::Begin("Hello, world!");
			{
				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);
		
				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color
		
				// Buttons return true when clicked (most widgets return true when edited/activated)
				if (ImGui::Button("Button")) {
					counter++;
				}
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);
		
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}
		*/

		/*
		// 3. Show another simple window.
        if (show_another_window){
            ImGui::Begin("Another Window", &show_another_window);
        	{
		        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            	ImGui::Text("Hello from another window!");
            	if (ImGui::Button("Close Me")){
            		show_another_window = false;
            	}
	        } ImGui::End();
		}
		*/
		
		midi_viewport->Draw();
		
		// MIDI UI
		bool MIDI_UI_OPEN = true;
		bool MIDI_UI_CHANNELS_OPEN = true;
		constexpr ImGuiWindowFlags MIDI_UI_FLAGS = ImGuiWindowFlags_None;
		if(ImGui::Begin("MIDI Viewport", &MIDI_UI_OPEN, MIDI_UI_FLAGS))
		{
			// if(ImGui::BeginMenuBar())
			// {
				// if (ImGui::BeginMenu("File")) {
				// 	if (ImGui::MenuItem("New")) {}				// TODO: Option "New"
				// 	if (ImGui::MenuItem("Save", "Ctrl+S")) {}	// TODO: Option "Save"
				// 	if (ImGui::MenuItem("Open", "Ctrl+O")) {}	// TODO: Option "Open"
				// 	if (ImGui::BeginMenu("Open Recent")){		// TODO: Option "Open Recent"
				// 		ImGui::MenuItem("test1.mid");
				// 		ImGui::MenuItem("test2.mid");
				// 		ImGui::MenuItem("test3.mid");
				// 		
				// 		ImGui::EndMenu(); /* END : Open Recent */ 
				// 	}
				// 	
				// 	ImGui::EndMenu(); /* END : FILE */ 
				// }
				//
				// if(ImGui::MenuItem("Options")) {}
				//
			// 	ImGui::EndMenuBar(); /* END : MENU BAR */ 
			// } 
		
			//TODO: Viewport renderer

			ImGui::Image(
				midi_viewport->viewport_tex_ptr,
				// tex_placeholder,
				// ImVec2(1280.0/2.0, 720.0/2.0)
				ImVec2(
					ImGui::GetCurrentWindow()->Size.x - ImGui::GetCurrentWindow()->WindowPadding.x,
					ImGui::GetCurrentWindow()->Size.y - ImGui::GetCurrentWindow()->WindowPadding.y - ImGui::GetCurrentWindow()->TitleBarHeight() - ImGui::GetCurrentWindow()->TitleBarHeight() 
				)
			);

			
			
			
		} /* END Window MIDI Viewport */ ImGui::End();
		
		if(ImGui::Begin("MIDI Channels", &MIDI_UI_CHANNELS_OPEN, MIDI_UI_FLAGS)) {
			
			
		} /* END : MIDI Channels */ ImGui::End();
	}

	void Render()
	{
		// Rendering
		ImGui::Render();

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Cleanup() {
		// midi_viewport.Delete();
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	/// UI ELEMENTS ///
	
	void UI_ShowMenu_File()
	{
		if (ImGui::MenuItem("New")) {}
		
		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
		
		if (ImGui::BeginMenu("Open Recent"))
		{
			ImGui::MenuItem("test1.mid");
			ImGui::MenuItem("test2.mid");
			ImGui::MenuItem("test3.mid");
			ImGui::MenuItem("..."); // TODO: Can replace with some scroll box?

			ImGui::EndMenu(); // Open Recent
		}
			
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			
		if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {}
		
		ImGui::Separator();
		
		if (ImGui::MenuItem("Quit", "Alt+F4")) {}
	}
}
