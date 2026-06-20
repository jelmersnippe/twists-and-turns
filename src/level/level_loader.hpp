#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <string>
#include <unordered_map>

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
void load_level(const std::string name, const ColorMap& map);
