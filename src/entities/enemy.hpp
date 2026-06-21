#pragma once

#include "core/animation_player.hpp"
#include "core/collision.hpp"
#include "core/transform.hpp"
#include "globals.hpp"

struct Enemy {
    Transform2D transform;
    Collider collider = Collider(
        Rect{.center = Vec2F{}, .size = Vec2F{.x = DEFAULT_SPRITE_SIZE * 0.7f, .y = DEFAULT_SPRITE_SIZE * 0.7f}});

    AnimationPlayer animation_player = AnimationPlayer({
        .sprite = SpriteInfo("enemy_walk", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 1, .y = 1}),
        .frame_count = 4,
    });

    Vec2F velocity = {};
    bool facing_left = false;
    float speed = 100;
    bool grounded = true;

    Vec2F target_position;
    Vec2F prev_position;
};
