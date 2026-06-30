#pragma once
#include "constants.h"
#include <raylib.h>
#include <vector>
#include <memory>
#include "raygui.h"
#include "Tile.h"

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

struct GameContext
{
	Camera2D camera = { 0 };
	GameState gameState = GameState::BuildTiles;
	TileActionType tileActionType = TileActionType::Walkway;
	std::vector<Rectangle> uiRects;

	//build drop down menu
	int buildDropDownActiveItem = 0;
	bool dropdownEditMode = false;
};

Textures loadAllTextures();

void unloadAllTextures(Textures& texture);

void DrawGrid2D(Vector2 center, int slices, int spacing, Color color);

float wrap(float min, float max, float value);

bool TrackedGuiButton(std::vector<Rectangle>& uiRects, Rectangle rect, const char* label);

bool TrackedGuiDropdownBox(std::vector<Rectangle>& uiRects, Rectangle bounds, const char* text, int* active, bool editMode);

TileActionType getPipeType(std::unique_ptr<Pipe> pipes[BUILD_SIZE][BUILD_SIZE], Vector2 mousePos);