#pragma once

#include "core/animation_player.hpp"
#include "core/collision.hpp"
#include "core/transform.hpp"
#include "globals.hpp"

const float PLAYER_SCALE = 3;

struct Player {
    Transform2D transform;
    Collider collider =
        Collider(Rect{.position = Vec2F{.x = -3, .y = -3}, .size = Vec2F{.x = 5, .y = 8}} * PLAYER_SCALE);

    AnimationPlayer animation_player = AnimationPlayer({
        .sprite = SpriteInfo("player", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 8, .y = 9},
                             {.x = PLAYER_SCALE, .y = PLAYER_SCALE}),
        .frame_count = 4,
    });

    Vec2F direction = {};
    float speed = 200;
};
