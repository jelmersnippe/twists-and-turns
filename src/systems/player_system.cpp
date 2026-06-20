#include "player_system.hpp"

#include "core/collision.hpp"
#include "core/input.hpp"
#include "game_state.hpp"
#include "globals.hpp"

bool check_collision_walls(const Vec2F& position, const Collider& collider, const std::vector<Wall>& walls) {
    for (const Wall& wall : walls) {
        const Collider player_collider = collider.at_position(position);
        const Collider wall_collider = wall.collider.at_position(wall.transform.position);
        if (collision_collider_collider(player_collider, wall_collider)) return true;
    }

    return false;
}

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

    if (input_frame.is_key_down(Key::Left)) player.direction += {.x = -1, .y = 0};
    if (input_frame.is_key_down(Key::Right)) player.direction += {.x = 1, .y = 0};

    float speed = player.speed;

    Vec2F velocity = player.direction.normalized() * speed * delta_time;
    velocity.y += GRAVITY * delta_time;
    if (velocity.y > GRAVITY) { velocity.y = GRAVITY; }

    Vec2F new_position = player.transform.position;

    new_position.x += velocity.x;
    const bool collision_x = check_collision_walls(new_position, player.collider, state.walls);

    new_position = player.transform.position;
    new_position.y += velocity.y;

    const bool collision_y = check_collision_walls(new_position, player.collider, state.walls);

    if (collision_x) velocity.x = 0;
    if (collision_y) velocity.y = 0;

    player.transform.position += velocity;

    // In air
    if (velocity.y != 0) {
        player.animation_player.current_frame = 1;
    } else {
        if (player.direction.x == 0 && player.direction.y == 0) {
            player.animation_player.stop();
        } else {
            player.animation_player.play();
        }

        player.animation_player.update(delta_time);
    }

    player.direction = {.x = 0, .y = 0};
}

void UpdatePlayers(GameState& state) {
    Update(state.player, state);
}

void DrawPlayers(const GameState& state) {
    bool flipped = state.player.direction.x < 0;

    state.player.animation_player.draw(
        Vec2F{.x = state.player.transform.position.x, .y = state.player.transform.position.y}, flipped);
}

void DrawPlayersDebug(const GameState& state) {
    if (!state.debug_enabled) return;

    state.player.collider.draw(state.player.transform.position);
}
