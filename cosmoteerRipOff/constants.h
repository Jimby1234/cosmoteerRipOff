#pragma once

constexpr float TILE_SIZE = 25.0f;
constexpr int BUILD_SIZE = 50;

enum class GameState {BuildTiles, BuildPipes, Play};

enum class TileActionType { 
	Walkway, Shield, 
	ThrusterUp, ThrusterRight, ThrusterDown, ThrusterLeft,
	PipeStart, 
	PipeIntersection,
	PipeBentDR, PipeBentLD, PipeBentUL, PipeBentUR,
	PipeEndUp, PipeEndRight, PipeEndDown, PipeEndLeft,
	PipeStraightUp, PipeStraightRight,
	PipeTURD, PipeTRDL, PipeDLU, PipeLUR,
	Remove };

constexpr int UP = 1 << 0; // 0001
constexpr int RIGHT = 1 << 1; // 0010
constexpr int DOWN = 1 << 2; // 0100
constexpr int LEFT = 1 << 3; // 1000