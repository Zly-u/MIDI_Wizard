#include "gui.h"

#include <memory>

#include <SDL_render.h>

#include "imgui.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "viewport.h"

#if MTR_ENABLED
	#include "minitrace.h"
#endif



void GUI::Init_Impl(SDL_Renderer* renderer) {
	midi_viewport = std::make_unique<viewport>(renderer);
	midi_viewport->OnStart();
	
	// ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();
}

void GUI::GenerateMIDI_Impl() {
	midi_viewport->GenerateMIDI();
}

void GUI::SetupDocking_Impl() {
	ImGuiDockNodeFlags dockspace_flags =
		// ImGuiDockNodeFlags_HiddenTabBar |
		// ImGuiDockNodeFlags_NoTabBar |
		ImGuiDockNodeFlags_AutoHideTabBar;
	
	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags docking_window_flags =
		ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		docking_window_flags |=
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
		docking_window_flags |= ImGuiWindowFlags_NoBackground;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	}
	
	if (ImGui::Begin("Main Dockspace", &dock_is_open, docking_window_flags)) {
		
		if (!opt_padding) {
			ImGui::PopStyleVar();
		}

		if (opt_fullscreen) {
			ImGui::PopStyleVar(2);
		}
		
		// Submit the DockSpace
		if (
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags & ImGuiConfigFlags_DockingEnable
		){
			const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else{
			ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
			ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::SmallButton("click here")) {
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			}
		}
		// if (ImGui::BeginMenuBar()) {
	 //        if (ImGui::BeginMenu("Options"))
	 //        {
	 //            // Disabling fullscreen would allow the window to be moved to the front of other windows,
	 //            // which we can't undo at the moment without finer window depth/z control.
	 //            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
	 //            ImGui::MenuItem("Padding", NULL, &opt_padding);
	 //            ImGui::Separator();
	 //
	 //            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
	 //            if (ImGui::MenuItem("Flag: NoDockingSplit",         "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))             { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
	 //            if (ImGui::MenuItem("Flag: NoUndocking",            "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
	 //            if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                   { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
	 //            if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))             { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
	 //            if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
	 //            ImGui::Separator();
	 //
	 //            
	 //        }ImGui::EndMenu();
	 //        HelpMarker(
	 //            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
	 //            "- Drag from window title bar or their tab to dock/undock." "\n"
	 //            "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
	 //            "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
	 //            "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
	 //            "This demo app has nothing to do with enabling docking!" "\n\n"
	 //            "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
	 //            "Read comments in ShowExampleAppDockSpace() for more details."
	 //        );
	 //
	 //        
	 //    }	ImGui::EndMenuBar();
	} ImGui::End(); /* Dockspace */
}


void GUI::Update_Impl() {
	midi_viewport->Update(1.f/60.f);
}

void GUI::Draw_Impl()
{
	MTR_SCOPE("GUI", "Draw_Impl");
	SetupDocking_Impl();

	// ImGUI part
	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	
			
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			GUI::UI_ShowMenu_File();
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
	
	///////////////////////////////////////
	
	midi_viewport->DrawWindow();
	// SDL Viewport rendering
	// Made pre and post for ease of coding graphics:tm:
	midi_viewport->SDL_PreDraw();
	midi_viewport->SDL_Draw();
	midi_viewport->SDL_PostDraw();
	
	///////////////////////////////////////
	
	if(ImGui::Begin("MIDI Channels", &MIDI_UI_CHANNELS_OPEN, MidiChannels_UI_flags)) {
		
		
	} ImGui::End(); /* MIDI Channels */ 
}

void GUI::Render_Impl()
{
	// Rendering
	ImGui::Render();

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void GUI::CleanUp_Impl() {
	midi_viewport = nullptr;
	
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

/// UI ELEMENTS ///

void GUI::UI_ShowMenu_File_Impl()
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

void GUI::HelpMarker_Impl(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}