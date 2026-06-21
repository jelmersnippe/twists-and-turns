#pragma once

#include "core/data.hpp"

const float FLOOR_CHECK_RANGE = 2;

const float TERMINAL_VELOCITY = 1000;
const float GRAVITY = 2000;

const int DEFAULT_SPRITE_SIZE = 64;
const Vec2F HALF_TILE = Vec2F{.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE} * 0.5f;

const int GRID_SIZE = 20;
const int CHUNK_SIZE = 10;
const int WORLD_CHUNK_SIZE = CHUNK_SIZE * DEFAULT_SPRITE_SIZE;

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 1200;
const Vec2F SCREEN_SIZE = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT};
const Vec2F SCREEN_CENTER = SCREEN_SIZE * 0.5f;

const int TARGET_FPS = 60;
