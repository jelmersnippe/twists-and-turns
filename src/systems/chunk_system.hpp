#pragma once

struct GameState;
struct Chunk;
struct Player;

void UpdateChunks(GameState& state);
void DrawChunks(const GameState& state);
void DrawChunksDebug(const GameState& state);

void rotate_chunk(Chunk& chunk, int angle, GameState& state);
