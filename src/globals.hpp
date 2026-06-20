#pragma once

#include "core/data.hpp"

const int DEFAULT_SPRITE_SIZE = 64;

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 1200;
const Vec2F SCREEN_SIZE = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT};
const Vec2F SCREEN_CENTER = SCREEN_SIZE * 0.5f;

const int TARGET_FPS = 60;
