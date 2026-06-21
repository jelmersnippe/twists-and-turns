#include "systems/chunk_system.hpp"
#include "core/collision.hpp"
#include "core/delta_time.hpp"
#include "game_state.hpp"
#include "globals.hpp"

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

void rotate_chunk(Chunk& chunk, int angle, Player& player) {
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
    if (chunk.has_player) {
        player.prev_position = player.transform.position;

        Vec2F player_offset = player.transform.position - chunk.world_rect.center;
        if (angle < 0) {
            player_offset = Vec2F{.x = player_offset.y, .y = -player_offset.x};
            player.target_position = player_offset + chunk.world_rect.center;
        } else {
            player_offset = Vec2F{.x = -player_offset.y, .y = player_offset.x};
            player.target_position = player_offset + chunk.world_rect.center;
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

void UpdateChunks(GameState& state) {
    for (Chunk& chunk : state.chunks) {
        chunk.hovered = collision_point_rect(get_mouse_world_position(state.camera), chunk.world_rect);
        chunk.has_player = collision_rect_rect(state.player.collider.rect, chunk.world_rect);

        if (!chunk.is_rotating) continue;

        chunk.current_rotation_time += get_delta_time();

        const float t = std::min(chunk.current_rotation_time / state.time_per_rotation, state.time_per_rotation);

        for (Wall& wall : chunk.walls) {
            rotate_element(chunk, wall.grid_info, wall.transform, t);
        }
        for (Spike& spike : chunk.spikes) {
            rotate_element(chunk, spike.grid_info, spike.transform, t);
        }

        if (chunk.has_player) {
            state.player.transform.position = Vec2F{
                .x = std::lerp(state.player.prev_position.x, state.player.target_position.x, t),
                .y = std::lerp(state.player.prev_position.y, state.player.target_position.y, t),
            };
        }

        if (chunk.current_rotation_time >= state.time_per_rotation) chunk.is_rotating = false;
    }
}

void DrawChunks(const GameState& state) {
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

        render_rectangle(chunk.world_rect, BLUE, true, true);
    }
}

void DrawChunksDebug(const GameState& state) {
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
}
