#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "scenes/pause_scene.hpp"
#include "systems/player_system.hpp"
#include <cmath>
#include <iostream>

namespace {

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    // DrawPlayers(state);

    for (const Wall& wall : state.walls) {
        render_sprite(wall.sprite, wall.transform);
    }

    // DrawPlayersDebug(state);

    for (const Wall& wall : state.walls) {
        wall.collider.draw(wall.transform.position);
    }

    EndMode2D();
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);

    if (!state.is_rotating && input_frame.is_key_pressed(Key::Q)) {
        state.is_rotating = true;
        state.current_rotation_time = 0.0f;
        state.previous_rotation = state.grid_rotation;
        state.grid_rotation = state.grid_rotation - 90 % 360;
        for (Wall& wall : state.walls) {
            wall.previous_rotation = wall.transform.rotation;
            wall.previous_grid_position = wall.grid_position;
            wall.grid_position = {.x = wall.grid_position.y, .y = state.grid_size - 1 - wall.grid_position.x};
        }
    }

    if (!state.is_rotating && input_frame.is_key_pressed(Key::E)) {
        state.is_rotating = true;
        state.current_rotation_time = 0.0f;
        state.previous_rotation = state.grid_rotation;
        state.grid_rotation = state.grid_rotation + 90 % 360;
        for (Wall& wall : state.walls) {
            wall.previous_rotation = wall.transform.rotation;
            wall.previous_grid_position = wall.grid_position;
            wall.grid_position = {
                .x = state.grid_size - 1 - wall.grid_position.y,
                .y = wall.grid_position.x,
            };
        }
    }
}

void Update(GameState& state) {
    state.camera.target.x = state.player.transform.position.x;
    state.camera.target.y = state.player.transform.position.y;

    UpdateInputs(state);

    if (state.is_rotating) {
        state.current_rotation_time += GetFrameTime();

        const float t = std::min(state.current_rotation_time / state.time_per_rotation, state.time_per_rotation);

        for (Wall& wall : state.walls) {
            const Vec2F previous_wall_offset = Vec2F::from_vec2(wall.previous_grid_position);
            const Vec2F target_wall_offset = Vec2F::from_vec2(wall.grid_position);

            wall.transform.position =
                Vec2F{
                    .x = std::lerp(previous_wall_offset.x, target_wall_offset.x, t),
                    .y = std::lerp(previous_wall_offset.y, target_wall_offset.y, t),
                } *
                DEFAULT_SPRITE_SIZE;

            wall.transform.rotation =
                wall.previous_rotation + std::lerp(state.previous_rotation, state.grid_rotation, t);
        }

        if (state.current_rotation_time >= state.time_per_rotation) { state.is_rotating = false; }
    }
    // UpdatePlayers(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    state.Reset();

    Player player = {.transform = {.position = SCREEN_CENTER, .rotation = 0}};
    state.player = player;
    state.camera.target.x = player.transform.position.x;
    state.camera.target.y = player.transform.position.y;

    const Vec2 box_size = {.x = 10, .y = 10};

    for (int x = 0; x < box_size.x; x++) {
        for (int y = 0; y < box_size.y; y++) {
            const bool should_place = (y == box_size.y - 3 && x == 1) ||
                                      (x == 0 || y == 0 || y == box_size.y - 1 || x == box_size.x - 1) ||
                                      (y == 3 && x == 5);

            if (!should_place) continue;

            const Vec2 grid_position = {.x = x, .y = y};

            const Wall wall = Wall{.transform{.position = Vec2F::from_vec2(grid_position * DEFAULT_SPRITE_SIZE)},
                                   .previous_grid_position = grid_position,
                                   .grid_position = grid_position};
            state.walls.push_back(wall);

            std::cout << "Wall at: " << wall.transform.position.x << ", " << wall.transform.position.y << std::endl;
        }
    }
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
