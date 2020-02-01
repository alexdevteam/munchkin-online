#ifndef MUNCHKIN_BATTLE_HPP__
#define MUNCHKIN_BATTLE_HPP__

#include "card.hpp"
#include "player.hpp"

#include <unordered_map>
#include <vector>

namespace munchkin {

struct Battle {
    Battle(State& s, PlayerPtr source) : state(&s), source_player(source) {}
    Battle() : state(nullptr), source_player(nullptr) {}
    Battle(const Battle&) = default;
    Battle& operator=(const Battle&) = default;

    State* state;
    PlayerPtr source_player;
    std::vector<PlayerPtr> helpers;
    
    // The number of treasures to draw at the end of the battle (int because sol2)
    int treasures_to_draw = 0;
    // The number of levels to obtain at the end of the battle (int because sol2)
    int levels_to_obtain = 0;
    // The offset for the total players' power. Can be negative.
    int player_power_offset = 0;
    int get_total_player_power();

    // Map where Card is the played card and size_t is the (monster) power of that card. Needed
    // because we need to keep track of each card's individual power.
    std::unordered_map<CardPtr, int> played_cards;

    // API Functions
    void add_card(CardPtr);
    void remove_card(CardPtr);
    void modify_card(CardPtr, int power);
    int get_card_power(CardPtr);
    std::vector<CardPtr> get_cards_played();

    // The offset for the total monsters' power. Can be negative.
    int monster_power_offset = 0;
    int get_total_monster_power();
};

} // namespace munchkin

#endif