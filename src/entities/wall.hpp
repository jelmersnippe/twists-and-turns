#pragma once

#include "core/collision.hpp"
#include "core/renderer.hpp"
#include "core/transform.hpp"
#include "globals.hpp"

struct Wall {
    Transform2D transform;
    Collider collider =
        Collider(Rect{.position = Vec2F{}, .size = Vec2F{.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}});
    SpriteInfo sprite = {"wall", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 1, .y = 1}};

    float previous_rotation;
    Vec2 previous_grid_position;
    Vec2 grid_position;
};
