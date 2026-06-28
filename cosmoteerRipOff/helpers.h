#pragma once
#include "constants.h"
#include <raylib.h>
#include <vector>
#include "raygui.h"

const char* gameStateToString(GameState gameState);

struct Textures
{
	Texture2D walkwayTile;
	Texture2D shieldTile;
	Texture2D thrusterOnTile;
	Texture2D thrusterOffTile;
	Texture2D pipeTJunction;
	Texture2D pipeStraight;
	Texture2D pipeBent;
	Texture2D pipeEnd;
	Texture2D pipeStart;
	Texture2D pipeIntersection;
};

Textures loadAllTextures();

void unloadAllTextures(Textures& texture);

void DrawGrid2D(Vector2 center, int slices, int spacing, Color color);

float wrap(float min, float max, float value);

bool TrackedGuiButton(std::vector<Rectangle>& uiRects, Rectangle rect, const char* label);

void buildUIRects(std::vector<Rectangle>& uiRects, GameState gameState);