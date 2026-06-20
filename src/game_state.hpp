#pragma once

#include "core/camera.hpp"
#include "entities/player.hpp"
#include "globals.hpp"

struct Targetable;

const int STARTING_CURRENCY = 10;

struct GameState {
    bool debug_enabled = false;
    bool should_exit = false;

    Player player;

    MainCamera camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                         .target = {.x = 0, .y = 0},
                         .rotation = 0.0f,
                         .zoom = 1.0f};

    void Reset() {
        player = {};
        camera.target = {.x = 0, .y = 0};
    }
};
