#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/input.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "scenes/pause_scene.hpp"
#include "systems/player_system.hpp"

namespace {

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawPlayers(state);
    DrawPlayersDebug(state);

    EndMode2D();
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
}
void Update(GameState& state) {
    state.camera.target.x = state.player.transform.position.x;
    state.camera.target.y = state.player.transform.position.y;

    UpdateInputs(state);
    UpdatePlayers(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    state.Reset();
    Player player = {.transform = {.position = SCREEN_CENTER, .rotation = 0}};
    state.camera.target.x = player.transform.position.x;
    state.camera.target.y = player.transform.position.y;
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
