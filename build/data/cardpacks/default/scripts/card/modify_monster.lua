local card = {}

local function has_value (tab, val)
    for index, value in ipairs(tab) do
        if value == val then
            return true
        end
    end

    return false
end

function card.on_play(self)
    local function selection_filter(card)
        local cards_played = game.current_battle:get_cards_played()
        local is_present_in_battle = has_value(cards_played, card)
        if not is_present_in_battle then return false end

        return card:get_def().is_monster
    end

    print("select a card...")
    local selected_card = selection.choose_card(selection_filter)
    print("card selected...")
    if selection ~= nil then
        game.current_battle:add_card(self)
        game.current_battle:modify_card(selected_card, card.properties.power_to_add)
        self.target_monster = selected_card
    end
end

function card.on_battle_end(self)
    -- Reset the target_monster so the card doesn't modify it on anhilate
    -- Also because it's good practice to reset the state once the card has finished doing its job
    self.target_monster = nil
end

function card.on_annihilate(self)
    if self.target_monster ~= nil then
        self.target_monster.modify_card(selected_card, -card.properties.power_to_add)
    end
end

return card