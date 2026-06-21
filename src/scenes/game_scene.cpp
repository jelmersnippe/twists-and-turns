#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/input.hpp"
#include "core/transform.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/pause_scene.hpp"
#include "systems/chunk_system.hpp"
#include "systems/player_system.hpp"
#include <algorithm>

namespace {

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawPlayers(state);
    DrawChunks(state);

    DrawPlayersDebug(state);
    DrawChunksDebug(state);

    EndMode2D();
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
    if (input_frame.is_key_pressed(Key::R)) SCENE_MANAGER.SetScene(state, GAME_SCENE);
    if (input_frame.is_key_pressed(Key::F5)) {
        state.current_level_index -= 1;
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }
    if (input_frame.is_key_pressed(Key::F6)) {
        state.current_level_index += 1;
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    }

    if (input_frame.is_key_pressed(Key::Q)) {
        for (Chunk& chunk : state.chunks) {
            if (!chunk.hovered) continue;

            rotate_chunk(chunk, -90, state.player);

            break;
        }
    }

    if (input_frame.is_key_pressed(Key::E)) {
        for (Chunk& chunk : state.chunks) {
            if (!chunk.hovered) continue;

            rotate_chunk(chunk, 90, state.player);

            break;
        }
    }
}

void Update(GameState& state) {
    UpdateChunks(state);

    if (std::ranges::any_of(state.chunks, [](const Chunk& chunk) { return chunk.is_rotating; })) return;

    UpdateInputs(state);

    UpdatePlayers(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    state.Reset();

    state.load_level();

    state.camera.target.x = state.player.transform.position.x;
    state.camera.target.y = state.player.transform.position.y;
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
