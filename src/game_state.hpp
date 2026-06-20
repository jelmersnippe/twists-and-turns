#pragma once

#include "core/camera.hpp"
#include "entities/player.hpp"
#include "entities/spike.hpp"
#include "entities/wall.hpp"
#include "globals.hpp"
#include <vector>

struct Chunk {
    Rect world_rect;
    Rect grid_rect;
    bool is_rotating = false;
    int previous_rotation = 0;
    int rotation = 0;
    std::vector<Wall> walls{};
    std::vector<Spike> spikes{};
    float current_rotation_time = 0.0f;

    bool hovered = false;
    bool can_rotate = true;
};

struct GameState {
    bool debug_enabled = false;
    bool should_exit = false;

    float time_per_rotation = 1.0f;

    std::vector<Chunk> chunks{};

    Player player;

    MainCamera camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                         .target = {.x = 0, .y = 0},
                         .rotation = 0.0f,
                         .zoom = 0.5f};

    void Reset() {
        player = {};
        chunks = {};
        camera.target = {.x = 0, .y = 0};
    }
};
