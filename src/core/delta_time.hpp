#pragma once

#include "raylib.h"
#include <algorithm>

inline float get_delta_time() {
    return std::min(GetFrameTime(), 0.05f);
}
