#pragma once

#include "core/collision.hpp"
#include "core/transform.hpp"
#include "entities/grid_element.hpp"
#include "globals.hpp"

struct Spike {
    Transform2D transform;
    Collider collider =
        Collider(Rect{.position = Vec2F{}, .size = Vec2F{.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}});
    SpriteInfo sprite = {"spike", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 1, .y = 1}};

    GridInfo grid_info;
};
