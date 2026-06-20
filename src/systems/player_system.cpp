#include "player_system.hpp"

#include "blueprints/particles.hpp"
#include "core/camera.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "core/sound.hpp"
#include "game_state.hpp"
#include "globals.hpp"

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

    if (input_frame.is_key_down(Key::W)) player.direction += {.x = 0, .y = -1};
    if (input_frame.is_key_down(Key::S)) player.direction += {.x = 0, .y = 1};
    if (input_frame.is_key_down(Key::A)) player.direction += {.x = -1, .y = 0};
    if (input_frame.is_key_down(Key::D)) player.direction += {.x = 1, .y = 0};

    if (player.direction.x == 0 && player.direction.y == 0) {
        player.animation_player.stop();
    } else {
        player.animation_player.play();
    }
    player.animation_player.update(delta_time);

    float speed = player.speed;

    Vec2F velocity = player.direction.normalized() * speed * delta_time;

    Vec2F new_position = player.transform.position;

    new_position.x += velocity.x;
    // TODO: Re-add collision checks
    // const CollisionResult collision_x = check_player_collision(state, new_position);

    new_position = player.transform.position;
    new_position.y += velocity.y;
    // TODO: Re-add collision checks
    // const CollisionResult collision_y = check_player_collision(state, new_position);

    // if (!collision_x.collided) player.position.x += velocity.x;
    // if (!collision_y.collided) player.position.y += velocity.y;

    player.direction = {.x = 0, .y = 0};
}

void UpdatePlayers(GameState& state) {
    Update(state.player, state);
}

void DrawPlayers(const GameState& state) {
    const Vec2F mouse_position = get_mouse_world_position(state.camera);
    Vec2F direction = state.player.transform.position.direction_to(mouse_position);
    bool flipped = direction.x < 0;

    state.player.animation_player.draw(
        Vec2F{.x = state.player.transform.position.x, .y = state.player.transform.position.y}, flipped);
}

void DrawPlayersDebug(const GameState& state) {
    if (!state.debug_enabled) return;

    state.player.collider.draw(state.player.transform.position);
}
