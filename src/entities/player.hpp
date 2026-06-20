#pragma once

#include "core/animation_player.hpp"
#include "core/collision.hpp"
#include "core/transform.hpp"
#include "globals.hpp"

const float FLOOR_CHECK_RANGE = 2;

struct Player {
    Transform2D transform;
    Collider collider = Collider(
        Rect{.position = Vec2F{}, .size = Vec2F{.x = DEFAULT_SPRITE_SIZE * 0.7f, .y = DEFAULT_SPRITE_SIZE * 0.7f}});

    AnimationPlayer animation_player = AnimationPlayer({
        .sprite = SpriteInfo("player_walk", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 1, .y = 1}),
        .frame_count = 4,
    });

    Vec2F velocity = {};
    bool facing_left = false;
    float speed = 200;
    float jump_force = 800;
    bool grounded = true;
};
