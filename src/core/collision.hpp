#pragma once

#include "core/data.hpp"
#include "core/renderer.hpp"
#include "raylib.h"

enum class ColliderType {
    Circle,
    Box
};

struct Collider {
    ColliderType type;

    // Box
    Rect rect = {};

    // Circle
    Vec2F center = {};
    float radius = 0;

    Collider(Vec2F center, float radius) : type(ColliderType::Circle), center(center), radius(radius) {}
    Collider(Rect rect) : type(ColliderType::Box), rect(rect) {}

    void draw(Vec2F position, Color color = RED) const {
        switch (this->type) {
            case ColliderType::Circle:
                render_circle(position + this->center, radius, color, true);
                break;
            case ColliderType::Box:
                render_rectangle(position + this->rect.center, this->rect.size, color, true, true);
                break;
        }
    }

    Collider at_position(Vec2F position) const {
        switch (this->type) {
            case ColliderType::Circle:
                return {this->center + position, this->radius};
            case ColliderType::Box:
                return Collider({.center = this->rect.center + position, .size = this->rect.size});
        }
    }
};

inline bool collision_point_circle(Vec2F point, Vec2F center, float radius) {
    return CheckCollisionPointCircle({.x = point.x, .y = point.y}, {.x = center.x, .y = center.y}, radius);
}

inline bool collision_point_rect(Vec2F point, Rect rect) {
    return CheckCollisionPointRec(
        {.x = point.x, .y = point.y},
        {.x = rect.top_left.x, .y = rect.top_left.y, .width = rect.size.x, .height = rect.size.y});
}

inline bool collision_rect_rect(Rect rect, Rect other) {
    return CheckCollisionRecs(
        {.x = other.top_left.x, .y = other.top_left.y, .width = other.size.x, .height = other.size.y},
        {.x = rect.top_left.x, .y = rect.top_left.y, .width = rect.size.x, .height = rect.size.y});
}

inline bool collision_circle_rect(Vec2F center, float radius, Rect rect) {
    return CheckCollisionCircleRec(
        center.to_raylib(), radius,
        {.x = rect.top_left.x, .y = rect.top_left.y, .width = rect.size.x, .height = rect.size.y});
}

inline bool collision_circle_circle(Vec2F center, float radius, Vec2F other_center, float other_radius) {
    return CheckCollisionCircles(center.to_raylib(), radius, other_center.to_raylib(), other_radius);
}

inline bool collision_point_collider(Vec2F point, Collider collider) {
    switch (collider.type) {
        case ColliderType::Circle:
            return collision_point_circle(point, collider.center, collider.radius);
            break;
        case ColliderType::Box:
            return collision_point_rect(point, collider.rect);
            break;
    }
}

inline bool collision_collider_collider(Collider collider, Collider other) {
    switch (collider.type) {
        case ColliderType::Circle:
            switch (other.type) {
                case ColliderType::Circle:
                    return collision_circle_circle(collider.center, collider.radius, other.center, other.radius);
                case ColliderType::Box:
                    return collision_circle_rect(collider.center, collider.radius, other.rect);
                    break;
            }
            break;
        case ColliderType::Box:
            switch (other.type) {
                case ColliderType::Circle:
                    return collision_circle_rect(collider.center, collider.radius, other.rect);
                case ColliderType::Box:
                    return collision_rect_rect(collider.rect, other.rect);
                    break;
            }
            break;
    }
}
