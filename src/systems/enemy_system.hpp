#pragma once

struct GameState;
struct Camera2D;

void UpdateEnemies(GameState& state);

void DrawEnemies(const GameState& state);
void DrawEnemiesDebug(const GameState& state);
