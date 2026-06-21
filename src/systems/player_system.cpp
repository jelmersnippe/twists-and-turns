#include "player_system.hpp"

#include "core/collision.hpp"
#include "core/delta_time.hpp"
#include "core/input.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "scenes/game_scene.hpp"
#include "systems/scene_manager.hpp"
#include <algorithm>

void Update(Player& player, GameState& state) {
    const float delta_time = get_delta_time();
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

    if (chunk == state.chunks.end()) {
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
        return;
    }

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
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }

    if (chunk->doors.size() > 0 && chunk->doors[0].opened &&
        check_collision_doors(player.transform.position, player.collider, chunk->doors)) {
        state.current_level_index = (state.current_level_index + 1) % state.levels.size();
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }

    if (std::ranges::any_of(state.enemies.data, [state](const Slot<Enemy>& slot) {
            return slot.alive &&
                   collision_collider_collider(state.player.collider.at_position(state.player.transform.position),
                                               slot.ref.collider.at_position(slot.ref.transform.position));
        })) {
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
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
