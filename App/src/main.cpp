// Dear ImGui: standalone example application for SDL2 + Vulkan

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.




#include "window_handler.h"
#include "gui.h"


int main(int /*argc*/, char** /*argv*/){
	printf("Program start\n");
	// Window Singleton
	window_handler WindowHandler;
	const int ws_result = WindowHandler.WindowSetup();
	if(ws_result != 0){
		return ws_result;
	}
	
	gui::Init(WindowHandler.SDL_Renderer_ptr);
	
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
        		event.window.windowID == SDL_GetWindowID(WindowHandler.SDL_Window_ptr))
        	{
        		done = true;
        	}
        }

    	// Start the ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
    	ImGui_ImplSDL2_NewFrame();
    	ImGui::NewFrame();

    	// Draw in frame
    	gui::DrawUI();
    	gui::Render();

    	// End frame
    	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    	SDL_RenderSetScale(
    		WindowHandler.SDL_Renderer_ptr,
    		WindowHandler.ImIO_ptr->DisplayFramebufferScale.x,
    		WindowHandler.ImIO_ptr->DisplayFramebufferScale.y
    	);
    	SDL_SetRenderDrawColor(
    		WindowHandler.SDL_Renderer_ptr,
    		static_cast<Uint8>(clear_color.x * 255),
    		static_cast<Uint8>(clear_color.y * 255),
    		static_cast<Uint8>(clear_color.z * 255),
    		static_cast<Uint8>(clear_color.w * 255)
    	);
    	SDL_RenderClear(WindowHandler.SDL_Renderer_ptr);
    	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    	// Display all drawn contents from the backbuffer
    	SDL_RenderPresent(WindowHandler.SDL_Renderer_ptr);
    }

	gui::Cleanup();
	WindowHandler.Cleanup();
	
    return 0;
}