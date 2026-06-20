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
#include <iostream>

namespace {

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawPlayers(state);

    for (const Wall& wall : state.walls) {
        render_sprite(wall.sprite, wall.transform.position);
    }

    DrawPlayersDebug(state);

    for (const Wall& wall : state.walls) {
        wall.collider.draw(wall.transform.position);
    }

    EndMode2D();
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
}
void Update(GameState& state) {
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

    Player player = {.transform = {.position = SCREEN_CENTER, .rotation = 0}};
    state.player = player;
    state.camera.target.x = player.transform.position.x;
    state.camera.target.y = player.transform.position.y;

    const Vec2 box_size = {.x = 20, .y = 10};

    for (int x = 0; x < box_size.x; x++) {
        const int wall_place_x = x - box_size.x / 2;

        for (int y = 0; y < box_size.y; y++) {
            const bool should_place = y == box_size.y - 3 && x == 1;
            if ((x != 0 && y != 0 && y != box_size.y - 1) && !should_place) continue;

            const int wall_place_y = y - box_size.y / 2;

            const Wall wall = Wall{.transform{.position = {.x = SCREEN_CENTER.x + DEFAULT_SPRITE_SIZE * wall_place_x,
                                                           .y = SCREEN_CENTER.y + DEFAULT_SPRITE_SIZE * wall_place_y}}};
            state.walls.push_back(wall);

            std::cout << "Wall at: " << wall.transform.position.x << ", " << wall.transform.position.y << std::endl;
        }
    }
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
