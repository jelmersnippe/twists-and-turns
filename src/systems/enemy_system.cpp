#include "enemy_system.hpp"

#include "core/collision.hpp"
#include "core/delta_time.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"

void Update(Slot<Enemy>& slot, GameState& state) {
    if (!slot.alive) return;

    Enemy& enemy = slot.ref;

    const float delta_time = get_delta_time();
    Vec2F velocity = enemy.velocity;

    velocity.x = 0;

    if (enemy.grounded) {
        if (enemy.facing_left) {
            velocity.x -= enemy.speed;
        } else {
            velocity.x += enemy.speed;
        }
    }

    velocity.y += GRAVITY * delta_time;
    if (velocity.y > TERMINAL_VELOCITY) velocity.y = TERMINAL_VELOCITY;

    enemy.transform.position.x += velocity.x * delta_time;

    const auto chunk = std::ranges::find_if(state.chunks, [enemy](const Chunk& chunk) {
        return collision_point_rect(enemy.transform.position, chunk.world_rect);
    });

    if (chunk == state.chunks.end()) {
        DestroyEntity(state.enemies, slot.handle);
        return;
    }

    if (velocity.x != 0 && check_collision_walls(enemy.transform.position, enemy.collider, chunk->walls)) {
        enemy.transform.position.x -= velocity.x * delta_time;
        velocity.x = 0;
        enemy.facing_left = !enemy.facing_left;
    }

    if (velocity.x != 0) enemy.facing_left = velocity.x < 0;

    enemy.transform.position.y += velocity.y * delta_time;
    if (check_collision_walls(enemy.transform.position, enemy.collider, chunk->walls)) {
        enemy.transform.position.y -= velocity.y * delta_time;

        if (velocity.y > 0) enemy.grounded = true;

        velocity.y = 0;
    }

    enemy.velocity = velocity;

    // In air
    if (velocity.y != 0) {
        enemy.animation_player.current_frame = 1;
    } else {
        if (enemy.velocity.x == 0) {
            enemy.animation_player.stop();
        } else {
            enemy.animation_player.play();
        }

        enemy.animation_player.update(delta_time);
    }

    if (check_collision_spikes(enemy.transform.position, enemy.collider, chunk->spikes)) {
        DestroyEntity(state.enemies, slot.handle);
    }
}

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        Update(slot, state);
    }
}

void DrawEnemies(const GameState& state) {
    for (const Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        slot.ref.animation_player.draw(slot.ref.transform, slot.ref.facing_left);
    }
}

void DrawEnemiesDebug(const GameState& state) {
    if (!state.debug_enabled) return;

    for (const Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        slot.ref.collider.draw(slot.ref.transform.position);
    }
}
