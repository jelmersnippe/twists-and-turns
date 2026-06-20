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

    Player player = {.transform = {.position = SCREEN_CENTER + Vec2F{.x = 100, .y = 200}, .rotation = 0}};
    state.player = player;
    state.camera.target.x = player.transform.position.x;
    state.camera.target.y = player.transform.position.y;

    const Vec2 chunks_to_build = {.x = GRID_SIZE / CHUNK_SIZE, .y = GRID_SIZE / CHUNK_SIZE};

    for (int chunk_x = 0; chunk_x < chunks_to_build.x; chunk_x++) {
        for (int chunk_y = 0; chunk_y < chunks_to_build.y; chunk_y++) {
            const Vec2F chunk_top_left = Vec2F{.x = static_cast<float>(chunk_x * WORLD_CHUNK_SIZE),
                                               .y = static_cast<float>(chunk_y * WORLD_CHUNK_SIZE)};

            const Vec2F chunk_center =
                chunk_top_left + Vec2F{.x = WORLD_CHUNK_SIZE * 0.5f, .y = WORLD_CHUNK_SIZE * 0.5f};

            Chunk current_chunk = {
                .world_rect = {.position = chunk_center, .size = {.x = WORLD_CHUNK_SIZE, .y = WORLD_CHUNK_SIZE}},
                .grid_rect = {.position = Vec2F{.x = static_cast<float>(chunk_x), .y = static_cast<float>(chunk_y)},
                              .size = {.x = CHUNK_SIZE, .y = CHUNK_SIZE}}};

            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    const Vec2 grid_position = {.x = x, .y = y};
                    const Vec2F world_position =
                        chunk_top_left + Vec2F::from_vec2(grid_position * DEFAULT_SPRITE_SIZE) + HALF_TILE;

                    if (x == 4 && y == 4) {
                        current_chunk.spikes.push_back(
                            Spike{.transform = {.position = world_position, .rotation = 90},
                                  .grid_info = {.previous_position = grid_position, .position = grid_position}});
                        continue;
                    }
                    if (x == 5 && y == 4) {
                        current_chunk.spikes.push_back(
                            Spike{.transform = {.position = world_position, .rotation = 180},
                                  .grid_info = {.previous_position = grid_position, .position = grid_position}});
                        continue;
                    }
                    if (x == 4 && y == 5) {
                        current_chunk.spikes.push_back(
                            Spike{.transform = {.position = world_position, .rotation = 270},
                                  .grid_info = {.previous_position = grid_position, .position = grid_position}});
                        continue;
                    }
                    if (x == 5 && y == 5) {
                        current_chunk.spikes.push_back(
                            Spike{.transform = {.position = world_position, .rotation = 0},
                                  .grid_info = {.previous_position = grid_position, .position = grid_position}});
                        continue;
                    }

                    const bool should_place = x == 0 || y == 0 || x == CHUNK_SIZE - 1 || y == CHUNK_SIZE - 1;
                    const bool should_not_place =
                        ((x == 0 || x == CHUNK_SIZE - 1) && (y == 3 || y == CHUNK_SIZE - 3)) ||
                        ((y == 0 || y == CHUNK_SIZE - 1) && (x == 3 || x == CHUNK_SIZE - 3));

                    if (should_not_place || !should_place) continue;

                    const Wall wall =
                        Wall{.transform = {.position = world_position},
                             .grid_info = {.previous_position = grid_position, .position = grid_position}};
                    current_chunk.walls.push_back(wall);
                }
            }

            state.chunks.push_back(current_chunk);
        }
    }
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
