-- api_wrapper.lua
-- Defines API functions pretty much generic to all game rules, such as iter_players
-- and wrappers for the state variable

-- Note: Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

-- munchkin_state

print = function(obj)
    local name = debug.getinfo(2).name
    internal_print(tostring(name and name or "anonymous"), tostring(obj))
end

function munchkin_game:iter_players()
    local i = -1
    return function()
        i = i + 1
        if i == self:get_player_count() then return end
        return self:get_player(i)
    end
end

function munchkin_game:next_player_turn()
    self:set_current_player((self:get_current_player().id + 1) % self:get_player_count())
    self.turn_number = self.turn_number + 1
end

-- input

selection = {}

function selection.choose_card(filter)
    filter = filter or function() return true end

    if(game:get_current_player().id == client.local_player_id) then
        coroutine.yield("choose_card", filter)
    end
    while game.last_event.type ~= event_type.card_clicked do
        -- Discard any data that we do not want
        coroutine.yield()
    end

    return game.last_event.card_involved
end

-- Utils

function wait_for_event(ev)
	repeat
		coroutine.yield()
	until game.last_event.type == ev
end

function wait_for_ticks(ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (ticks_before + ticks)
end

-- Waits for a number or ticks, or until an event has happened.
-- Returns true if waited for all the ticks, false if the event happened first.
function wait_for_ticks_or_event(ev, ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (game:get_ticks() + ticks) or game.last_event.type == ev
	return game:get_ticks() >= (game:get_ticks() + ticks)
end