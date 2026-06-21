#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

struct LevelInfo {
    int index;
    Vec2 size;
    Vec2 chunk_size;
    std::vector<Color> colors;
};

struct LevelFileInfo {
    int index;
    int chunk_size_x;
    int chunk_size_y;
};

std::vector<LevelInfo> load_levels();
