#include "player_system.hpp"

#include "core/collision.hpp"
#include "core/input.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "scenes/game_over_scene.hpp"
#include "scenes/game_scene.hpp"
#include "systems/scene_manager.hpp"

bool check_collision_walls(const Vec2F& position, const Collider& collider, const std::vector<Wall>& walls) {
    for (const Wall& wall : walls) {
        const Collider player_collider = collider.at_position(position);
        const Collider wall_collider = wall.collider.at_position(wall.transform.position);
        if (collision_collider_collider(player_collider, wall_collider)) return true;
    }

    return false;
}

bool check_collision_spikes(const Vec2F& position, const Collider& collider, const std::vector<Spike>& spikes) {
    for (const Spike& spike : spikes) {
        const Collider player_collider = collider.at_position(position);
        const Collider spike_collider = spike.collider.at_position(spike.transform.position);
        if (collision_collider_collider(player_collider, spike_collider)) return true;
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

    velocity.y += GRAVITY * delta_time;
    if (velocity.y > TERMINAL_VELOCITY) velocity.y = TERMINAL_VELOCITY;

    player.transform.position.x += velocity.x * delta_time;

    const auto chunk = std::ranges::find_if(state.chunks, [&player](const Chunk& chunk) {
        return collision_point_rect(player.transform.position, chunk.world_rect);
    });

    assert(chunk != state.chunks.end() && "Player not inside of a chunk");

    if (check_collision_walls(player.transform.position, player.collider, chunk->walls)) {
        player.transform.position.x -= velocity.x * delta_time;
        velocity.x = 0;
    }

    if (velocity.x != 0) player.facing_left = velocity.x < 0;

    player.transform.position.y += velocity.y * delta_time;
    if (check_collision_walls(player.transform.position, player.collider, chunk->walls)) {
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

    if (check_collision_spikes(player.transform.position, player.collider, chunk->spikes)) {
        SCENE_MANAGER.SetScene(state, GAME_OVER_SCENE);
    }
}

void UpdatePlayers(GameState& state) {
    Update(state.player, state);
}

void DrawPlayers(const GameState& state) {
    state.player.animation_player.draw(state.player.transform, state.player.facing_left);
}

void DrawPlayersDebug(const GameState& state) {
    if (!state.debug_enabled) return;

    state.player.collider.draw(state.player.transform.position);
}
