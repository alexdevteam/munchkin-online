#include "game_wrapper.hpp"

#include <memory>
#include <optional>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <sdl/SDL.h>

#include <glad/glad.h>

namespace munchkin {

	GameWrapper::GameWrapper(size_t window_w, size_t window_h, size_t players_count, size_t ai_count, std::string gamerules_path) :
		game(players_count, window_w, window_h, gamerules_path),
		renderer(game),
		input_binder(game),
		state_debugger(game),
		ai_manager(std::move(create_ai_manager(players_count, ai_count)))
	{}

void GameWrapper::main_loop(SDL_Window* window) {
	bool done = false;
	ImGuiIO& io = ImGui::GetIO();

	do {
		// From imgui/examples/example_sdl_opengl3/main.cpp:
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED
				&& event.window.windowID == SDL_GetWindowID(window)) {
				glViewport(0, 0, event.window.data1, event.window.data2);
				renderer.on_resize(event.window.data1, event.window.data2);
			}
			else if (event.type == SDL_KEYDOWN && !io.WantCaptureKeyboard && event.key.keysym.scancode == SDL_SCANCODE_K && event.key.repeat == 0)
				show_debugger = !show_debugger;
		}

		// Start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.render_frame();
		renderer.blit(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (show_debugger)
			state_debugger.render();
		ImGui::Render();
		//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);

		input::update();
		input_binder.tick();
		ai_manager.tick();
		game.tick();
	} while (!done);

	std::cout << std::boolalpha;
	std::cout << "Game over: " << game.ended() << std::endl;

	std::cout << "Levels: " << std::endl;
	for (int i = 0; i < 4; ++i) {
		std::cout << i << ": " << game.get_state().players[i].level << "\n";
	}

	munchkin::renderer::SpriteRenderer::deallocate();
	munchkin::renderer::FontRenderer::deallocate();
}

games::AIManager GameWrapper::create_ai_manager(size_t players_count, size_t ai_count)
{
	if (ai_count > players_count)
		throw std::runtime_error("More AI given than players available");
	else if (ai_count == players_count)
	{
		// No local player, all AIs
		game.local_player_id = -1;

		// Add AIs
		std::vector<size_t> ais(players_count);
		std::iota(ais.begin(), ais.end(), 0);
		return games::AIManager(game.state, ais);
	}
	else
	{
		// Local player is the 0th player (Assuming not online play)
		game.local_player_id = 0;

		// Add AIs
		std::vector<size_t> ais(players_count - 1);
		std::iota(ais.begin(), ais.end(), 1);
		return games::AIManager(game.state, ais);
	}
}

}