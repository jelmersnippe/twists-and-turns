#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/collision.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "core/transform.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "scenes/pause_scene.hpp"
#include "systems/player_system.hpp"

namespace {

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawPlayers(state);

    for (const Chunk& chunk : state.chunks) {
        for (const Wall& wall : chunk.walls) {
            render_sprite(wall.sprite, wall.transform);
        }
        for (const Spike& spike : chunk.spikes) {
            render_sprite(spike.sprite, spike.transform);
        }
    }
    for (const Chunk& chunk : state.chunks) {
        if (!chunk.hovered) continue;

        Color color = chunk.can_rotate ? BLUE : RED;
        render_rectangle(chunk.world_rect, color, true, true);
    }

    DrawPlayersDebug(state);

    if (state.debug_enabled) {
        for (const Chunk& chunk : state.chunks) {
            for (const Wall& wall : chunk.walls) {
                wall.collider.draw(wall.transform.position);
            }
            for (const Spike& spike : chunk.spikes) {
                spike.collider.draw(spike.transform.position);
            }
        }
    }

    EndMode2D();
}

void rotate_tile(const Chunk& chunk, GridInfo& grid_info, Transform2D& transform, float angle) {
    grid_info.previous_rotation = transform.rotation;
    grid_info.previous_position = grid_info.position;

    if (angle < 0) {
        grid_info.position =
            Vec2{.x = grid_info.position.y, .y = static_cast<int>(chunk.grid_rect.size.y) - 1 - grid_info.position.x};
    } else {
        grid_info.position =
            Vec2{.x = static_cast<int>(chunk.grid_rect.size.x) - 1 - grid_info.position.y, .y = grid_info.position.x};
    }
}

void rotate_chunk(Chunk& chunk, int angle) {
    if (chunk.is_rotating) return;

    chunk.is_rotating = true;
    chunk.current_rotation_time = 0.0f;
    chunk.previous_rotation = chunk.rotation;
    chunk.rotation = chunk.rotation + angle;

    for (Wall& wall : chunk.walls) {
        rotate_tile(chunk, wall.grid_info, wall.transform, static_cast<float>(angle));
    }
    for (Spike& spike : chunk.spikes) {
        rotate_tile(chunk, spike.grid_info, spike.transform, static_cast<float>(angle));
    }
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
    if (input_frame.is_key_pressed(Key::R)) SCENE_MANAGER.SetScene(state, GAME_SCENE);
    if (input_frame.is_key_pressed(Key::F5)) {
        state.current_level_index -= 1;
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }
    if (input_frame.is_key_pressed(Key::F6)) {
        state.current_level_index += 1;
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }

    if (input_frame.is_key_pressed(Key::Q)) {
        for (Chunk& chunk : state.chunks) {
            if (!chunk.hovered || !chunk.can_rotate) continue;

            rotate_chunk(chunk, -90);

            break;
        }
    }

    if (input_frame.is_key_pressed(Key::E)) {
        for (Chunk& chunk : state.chunks) {
            if (!chunk.hovered || !chunk.can_rotate) continue;

            rotate_chunk(chunk, 90);

            break;
        }
    }
}

void rotate_element(const Chunk& chunk, const GridInfo& grid_info, Transform2D& transform, const float t) {
    const Vec2F previous_offset = Vec2F::from_vec2(grid_info.previous_position);
    const Vec2F target_offset = Vec2F::from_vec2(grid_info.position);

    transform.position = chunk.world_rect.top_left +
                         (Vec2F{
                              .x = std::lerp(previous_offset.x, target_offset.x, t),
                              .y = std::lerp(previous_offset.y, target_offset.y, t),
                          } *
                          DEFAULT_SPRITE_SIZE) +
                         HALF_TILE;

    transform.rotation = grid_info.previous_rotation + std::lerp(chunk.previous_rotation, chunk.rotation, t);
}

void Update(GameState& state) {
    bool is_rotating = false;

    for (Chunk& chunk : state.chunks) {
        chunk.hovered = collision_point_rect(get_mouse_world_position(state.camera), chunk.world_rect);
        chunk.can_rotate = !collision_point_rect(state.player.transform.position, chunk.world_rect);

        if (!chunk.is_rotating) continue;

        is_rotating = true;

        chunk.current_rotation_time += GetFrameTime();

        const float t = std::min(chunk.current_rotation_time / state.time_per_rotation, state.time_per_rotation);

        for (Wall& wall : chunk.walls) {
            rotate_element(chunk, wall.grid_info, wall.transform, t);
        }
        for (Spike& spike : chunk.spikes) {
            rotate_element(chunk, spike.grid_info, spike.transform, t);
        }

        if (chunk.current_rotation_time >= state.time_per_rotation) chunk.is_rotating = false;
    }

    if (is_rotating) return;

    state.camera.target.x = state.player.transform.position.x;
    state.camera.target.y = state.player.transform.position.y;

    UpdateInputs(state);

    UpdatePlayers(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    state.Reset();

    state.load_level();

    state.camera.target.x = state.player.transform.position.x;
    state.camera.target.y = state.player.transform.position.y;
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
