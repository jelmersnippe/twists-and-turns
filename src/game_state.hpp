#pragma once

#include "core/camera.hpp"
#include "entities/player.hpp"
#include "entities/spike.hpp"
#include "entities/wall.hpp"
#include "globals.hpp"
#include "level/level_loader.hpp"
#include "raylib.h"
#include <vector>

struct ColorHash {
    std::size_t operator()(const Color& c) const { return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a; }
};

struct ColorEqual {
    bool operator()(const Color& a, const Color& b) const {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    }
};

using ColorMapping = void (*)(Vec2 position);
using ColorMap = std::unordered_map<Color, ColorMapping, ColorHash, ColorEqual>;

struct Chunk {
    Rect world_rect;
    Rect grid_rect;
    bool is_rotating = false;
    int previous_rotation = 0;
    int rotation = 0;
    std::vector<Wall> walls{};
    std::vector<Spike> spikes{};
    float current_rotation_time = 0.0f;

    bool has_player = false;
    bool hovered = false;
};

struct GameState {
    std::vector<LevelInfo> levels = {};
    bool debug_enabled = false;
    bool should_exit = false;

    int current_level_index = 0;

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

    void load_level() {
        this->chunks = {};

        assert(this->current_level_index < this->levels.size() && "Tried to load level out of bounds");

        const LevelInfo& info = levels[this->current_level_index];

        const Vec2F world_chunk_size = Vec2F::from_vec2(info.chunk_size) * DEFAULT_SPRITE_SIZE;

        for (int chunk_y = 0; chunk_y < info.size.y / info.chunk_size.y; chunk_y++) {
            for (int chunk_x = 0; chunk_x < info.size.x / info.chunk_size.x; chunk_x++) {
                const Vec2 chunk_grid_top_left = {.x = chunk_x * info.chunk_size.x, .y = chunk_y * info.chunk_size.y};

                const Vec2F chunk_world_top_left = Vec2F::from_vec2(chunk_grid_top_left) * DEFAULT_SPRITE_SIZE;

                const Vec2F chunk_center = chunk_world_top_left + (world_chunk_size * 0.5f);

                Chunk current_chunk = {.world_rect = {.center = chunk_center, .size = world_chunk_size},
                                       .grid_rect = {.center = Vec2F::from_vec2(chunk_grid_top_left) +
                                                               Vec2F::from_vec2(info.chunk_size) * 0.5f,
                                                     .size = Vec2F::from_vec2(info.chunk_size)}};

                for (int y = 0; y < info.chunk_size.y; y++) {
                    for (int x = 0; x < info.chunk_size.x; x++) {
                        const Vec2 grid_position = chunk_grid_top_left + Vec2{.x = x, .y = y};
                        const Vec2F world_position = Vec2F::from_vec2(grid_position) * DEFAULT_SPRITE_SIZE + HALF_TILE;

                        // Colors is a straight array so we have to convert to row-oriented index
                        const int i = grid_position.y * info.size.x + grid_position.x;
                        const Color& color = info.colors[i];

                        if (color.a == 0) continue;

                        if (ColorIsEqual(color, Color(0, 0, 0, 255))) {
                            const Wall wall = Wall{
                                .transform = {.position = world_position},
                                .grid_info = {.previous_position = {.x = x, .y = y}, .position = {.x = x, .y = y}}};
                            current_chunk.walls.push_back(wall);
                        } else if (ColorIsEqual(color, Color(0, 255, 0, 255))) {
                            const Player player = Player{.transform = {.position = world_position}};

                            this->player = player;
                            current_chunk.has_player = true;
                        }
                    }
                }

                this->chunks.push_back(current_chunk);
            }
        }
    }
};
