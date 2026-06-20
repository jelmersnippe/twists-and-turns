#include "player_system.hpp"

#include "core/collision.hpp"
#include "core/input.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"

bool check_collision_walls(const Vec2F& position, const Collider& collider, const std::vector<Wall>& walls) {
    for (const Wall& wall : walls) {
        const Collider player_collider = collider.at_position(position);
        const Collider wall_collider = wall.collider.at_position(wall.transform.position);
        if (collision_collider_collider(player_collider, wall_collider)) return true;
    }

    return false;
}

const float TERMINAL_VELOCITY = 1000;
const float GRAVITY = 2000;

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();
    Vec2F velocity = player.velocity;

    velocity.x = 0;

    if (input_frame.is_key_down(Key::Left)) velocity.x += -player.speed;
    if (input_frame.is_key_down(Key::Right)) velocity.x += player.speed;

    if (player.grounded && input_frame.is_key_pressed(Key::Space)) {
        velocity.y = -player.jump_force;
        player.grounded = false;
    }

    velocity.y += GRAVITY * delta_time;
    if (velocity.y > TERMINAL_VELOCITY) velocity.y = TERMINAL_VELOCITY;

    player.transform.position.x += velocity.x * delta_time;

    if (check_collision_walls(player.transform.position, player.collider, state.walls)) {
        player.transform.position.x -= velocity.x * delta_time;
        velocity.x = 0;
    }

    if (velocity.x != 0) player.facing_left = velocity.x < 0;

    player.transform.position.y += velocity.y * delta_time;
    if (check_collision_walls(player.transform.position, player.collider, state.walls)) {
        player.transform.position.y -= velocity.y * delta_time;

        if (velocity.y > 0) player.grounded = true;

        velocity.y = 0;
    }

    player.velocity = velocity;

    // In air
    if (velocity.y != 0) {
        player.animation_player.current_frame = 1;
    } else {
        if (player.velocity.x == 0) {
            player.animation_player.stop();
        } else {
            player.animation_player.play();
        }

        player.animation_player.update(delta_time);
    }
}

void UpdatePlayers(GameState& state) {
    Update(state.player, state);
}

void DrawPlayers(const GameState& state) {
    state.player.animation_player.draw(
        Vec2F{.x = state.player.transform.position.x, .y = state.player.transform.position.y},
        state.player.facing_left);
}

void DrawPlayersDebug(const GameState& state) {
    if (!state.debug_enabled) return;

    state.player.collider.draw(state.player.transform.position);
}
