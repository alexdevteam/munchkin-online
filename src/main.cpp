﻿#include <iostream>
#define SDL_MAIN_HANDLED

#include "api/munchkin.hpp"
#include "renderer/state_debugger.hpp"
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <sdl/SDL.h>

#include <glad/glad.h>

#include "renderer/game_renderer.hpp"

int main()
{
	munchkin::Game game(4);
	munchkin::StateDebugger debugger(game.get_state());
	game.get_state().add_cardpack("data/cardpacks/default/cards.json");

	std::cout << "Cards loaded: " << game.get_state().carddefs.size() << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "SDL Init error: " << SDL_GetError();
		return -1;
	}


	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	bool err = gladLoadGL() == 0;

	if (err)
	{
		std::cerr << "Failed to initialize GLAD!\n" << std::endl;
		return -1;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	munchkin::GameRenderer renderer(game, io.DisplaySize.x, io.DisplaySize.y);

	bool done = false;
	do {
		// From imgui/examples/example_sdl_opengl3/main.cpp:
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;

			if (game.ended())
				break;
		}

		// Start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		debugger.render();
		renderer.render_frame();

		// bind default framebuffer so Imgui is rendered directly to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

		game.tick();
	} while (!done);
	

	std::cout << std::boolalpha;
	std::cout << "Game over: " << game.ended() << std::endl;

	std::cout << "Levels: " << std::endl;
	for (int i = 0; i < 4; ++i) {
		std::cout << i << ": " << game.get_state().players[i].level << "\n";
	}

	return 0;
}
