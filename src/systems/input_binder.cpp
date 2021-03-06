#include "systems/input_binder.hpp"
#include "game.hpp"

#include "input/input.hpp"
#include "renderer/sprite_renderer.hpp"
#include "game_wrapper.hpp"

#include <algorithm>
#include <iostream>
#include <set>

namespace munchkin {
namespace systems {

InputBinder::InputBinder(GameWrapper& w) : wrapper(&w) {}

void InputBinder::tick() {
    // Interacting with cards //

    float frame_time = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = frame_time - last_frame_time;
    last_frame_time = frame_time;

    auto* game = &wrapper->game;
    math::Vec2D m_pos =
        (input::get_mouse_pos() +
         game->camera.offset * math::Vec2D{(float)game->window_w, -(float)game->window_h} / 2.f -
         math::Vec2D{0, (float)game->window_h}) *
        math::Vec2D{1, -1};
    for (auto& sprite : game->card_sprites) {
        Card& sprite_card = *sprite.get_card_ptr();
        if (sprite.get_rect().contains(m_pos)) {
            sprite.is_being_hovered = true;
            game->current_hovered_sprite = &sprite;
            if (input::has_mousebutton_been_clicked(input::MouseButton::left)) {
                game->push_event(FlowEvent{FlowEvent::EventType::card_clicked, sprite_card,
                                           game->local_player_id});
                break;
            }
        } else
        {
            if(game->current_hovered_sprite == &sprite)
                game->current_hovered_sprite = nullptr;
            sprite.is_being_hovered = false;
        }
    }

    // Panning //

    constexpr float pan_speed = 0.1f;

    // only enable panning if left mouse button is clicked
    if (input::is_mousebutton_pressed(input::MouseButton::left) & last_mouse_state.button_flagmap &
        SDL_BUTTON(SDL_BUTTON_LEFT)) {
        math::Vec2D offset = input::get_mouse_pos() -
                             math::Vec2D{(float)last_mouse_state.x, (float)last_mouse_state.y};

        wrapper->game.camera.offset.x -= offset.x * pan_speed * delta_time;
        wrapper->game.camera.offset.y += offset.y * pan_speed * delta_time;
    }

    last_mouse_state = input::get_current_mouse_state();
}

} // namespace systems
} // namespace munchkin