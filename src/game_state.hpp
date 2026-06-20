#pragma once

#include "core/camera.hpp"
#include "entities/player.hpp"
#include "entities/wall.hpp"
#include "globals.hpp"
#include <vector>

struct GameState {
    bool debug_enabled = true;
    bool should_exit = false;

    Player player;
    std::vector<Wall> walls{};

    MainCamera camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                         .target = {.x = 0, .y = 0},
                         .rotation = 0.0f,
                         .zoom = 1.0f};

    void Reset() {
        player = {};
        walls = {};
        camera.target = {.x = 0, .y = 0};
    }
};
