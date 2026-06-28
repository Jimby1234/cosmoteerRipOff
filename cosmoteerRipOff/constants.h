#pragma once

constexpr float TILE_SIZE = 25.0f;
constexpr int BUILD_SIZE = 50;

enum class GameState {BuildTiles, BuildPipes, Play};

enum class TileActionType { Walkway, Shield, ThrusterUp, ThrusterRight, ThrusterDown, ThrusterLeft, Remove };