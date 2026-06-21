#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <vector>

struct LevelInfo {
    int index;
    Vec2 size;
    int chunk_size;
    std::vector<Color> colors;
};

struct LevelFileInfo {
    int index;
    int chunk_size;
};

std::vector<LevelInfo> load_levels();
