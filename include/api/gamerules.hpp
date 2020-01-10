#ifndef MUNCHKIN_GAMERULES_HPP__
#define MUNCHKIN_GAMERULES_HPP__

#include <string>
#include <sol/sol.hpp>

// Defines the default gamerules used.
#define DEFAULT_GAMERULES_PATH "data/gamerules/default/"
#define GAMERULES_GAMEFLOW_FILENAME "game_flow.lua"

namespace munchkin {
	class State;

	struct GameRules {
		GameRules(State& state, std::string path);

		sol::coroutine game_flow;
	};
}

#endif