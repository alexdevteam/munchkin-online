// carddata.cpp test -- Checks if card tables and metatables work
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "api/card.hpp"
#include "api/carddef.hpp"
#include "api/state.hpp"

#include <sol/sol.hpp>

#include <iostream>

using namespace munchkin;

TEST_CASE("card metatables") {
	// Player count shouldn't matter in this test
	State state(5);

	// Create a card definition from metatable.lua, that defines a single function, set_local_var.
	CardDef example_def(state.lua, "data/cardpacks/test/scripts/metatable.lua", "Metatable Test", "Tests metatables");

	CardPtr card1 = &state.add_card(example_def);
	CardPtr card2 = &state.add_card(example_def);

	REQUIRE(&card1->get_def() == &card2->get_def());
	REQUIRE((sol::object)example_def.metatable["set_local_var"] != sol::lua_nil);
	REQUIRE((sol::object)card1->get_data_variable("set_local_var") != sol::lua_nil);

	SUBCASE("using set_local_var in card1 sets a variable in card1")
	{
		// Should set card1.variable to true and return true
		bool result = card1->execute_function("set_local_var").as<bool>();

		CHECK(result);

		SUBCASE("using set_local_var in card1 does NOT set the metatable's variable")
		{
			CHECK((sol::object)example_def.metatable["variable"] == sol::lua_nil);
		}

		SUBCASE("using set_local_var in card1 does NOT set card2's variable")
		{
			CHECK(card2->get_data_variable("variable") == sol::lua_nil);
		}
	}
}

TEST_CASE("State::current_battle") {
	// Player count shouldn't matter in this test
	State state(5);

	CardDef def(state.lua, "data/cardpacks/test/scripts/battle.lua", "current_battle Test", "Tests the current battle variable");
	CardPtr def_instance = &state.add_card(def);

	CHECK(def_instance->execute_function("test_battle_1"));
	CHECK(def_instance->execute_function("test_battle_2"));
	CHECK(def_instance->execute_function("test_battle_3"));
}

#include "api/card_loader.hpp"

TEST_CASE("json loader") {
	// Player count shouldn't matter in this test
	State state(5);

	SUBCASE("load all carddefs") {
		std::vector<CardDef> defs(load_cards("data/cardpacks/test/cards.json", state.lua));

		CHECK(defs.size() >= 3);

		SUBCASE("load card properties") {
			CardDef* def = nullptr;
			for(auto & d : defs) {
				if (d.name == "Property Test")
					def = &d;
			}
			CHECK(def != nullptr);
			
			CHECK(((sol::table)def->metatable["properties"]).size() == 3);

			CardPtr test = state.add_card(*def);
			CHECK(test->execute_function("test_text"));
			CHECK(test->execute_function("test_number"));
			CHECK(test->execute_function("test_boolean"));
		}
	}
}

TEST_CASE("card pointers") {
	// Player count shouldn't matter in this test
	State state(5);
	REQUIRE(state.last_id == 0);

	CardDef def(state.lua, "data/cardpacks/test/scripts/metatable.lua", "CardPtr Test", "Tests the CardPtr struct");
	CardDef different_def(state.lua, "data/cardpacks/test/scripts/metatable.lua", "A different CardPtr Test", "Tests the CardPtr struct, again");
	CardPtr card1 = &state.add_card(def);
	CHECK(card1.card_id == card1->get_id());
	CardPtr card2 = &state.add_card(def);
	CHECK(card2.card_id == card2->get_id());
	CardPtr card3 = &state.add_card(different_def);
	CHECK(card3.card_id == card3->get_id());

	CHECK(&card1->get_def() == &card2->get_def());
	CHECK(&card2->get_def() != &card3->get_def());

	CardPtr indirect_card1 = &*card1;

	CHECK(&indirect_card1->get_def() == &card1->get_def());
	CHECK(&indirect_card1->get_def() == &card2->get_def());
	CHECK(&indirect_card1->get_def() != &card3->get_def());

}

TEST_CASE("play stages") {
	// Player count shouldn't matter in this test
	State state(5);

	SUBCASE("load all carddefs") {
		std::vector<CardDef> defs(load_cards("data/cardpacks/test/cards.json", state.lua));

		CardDef* def = nullptr;
		for (auto& d : defs) {
			if (d.name == "Play Stages Test")
				def = &d;
		}

		REQUIRE(def != nullptr);

		CHECK(def->play_stages.size() == 2);
		CHECK(def->play_stages[0] == "EQUIP_STUFF_AND_OPEN_DUNGEON");
		CHECK(def->play_stages[1] == "CHARITY");
	}
}