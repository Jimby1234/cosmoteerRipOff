#include "helpers.h"
#include <raylib.h>
#include "raygui.h"
#include <rlgl.h>
#include <cmath>
#include <vector>
#include "Tile.h"
#include "constants.h"

const char* gameStateToString(GameState state) {
    switch (state) {
    case GameState::BuildTiles: return "buildT";
    case GameState::BuildPipes: return "buildP";
    case GameState::Play:  return "play";
    default:               return "unknown";
    }
}

Textures loadAllTextures()
{
    Textures t;

    Image walkwayTile = LoadImage("images/walkwayTile.png");
    t.walkwayTile = LoadTextureFromImage(walkwayTile);
    UnloadImage(walkwayTile);

    Image shieldTile = LoadImage("images/metalShield.png");
    t.shieldTile = LoadTextureFromImage(shieldTile);
    UnloadImage(shieldTile);

    Image thrusterOnTile = LoadImage("images/ThrusterOn.png");
    t.thrusterOnTile = LoadTextureFromImage(thrusterOnTile);
    UnloadImage(thrusterOnTile);

    Image thrusterOffTile = LoadImage("images/ThrusterOff.png");
    t.thrusterOffTile = LoadTextureFromImage(thrusterOffTile);
    UnloadImage(thrusterOffTile);

    Image pipeTJunction = LoadImage("images/pipeTJunction.png");
    t.pipeTJunction = LoadTextureFromImage(pipeTJunction);
    UnloadImage(pipeTJunction);

    Image pipeStraight = LoadImage("images/pipeStraight.png");
    t.pipeStraight = LoadTextureFromImage(pipeStraight);
    UnloadImage(pipeStraight);

    Image pipeBent = LoadImage("images/pipeBent.png");
    t.pipeBent = LoadTextureFromImage(pipeBent);
    UnloadImage(pipeBent);

    Image pipeEnd = LoadImage("images/pipeEnd.png");
    t.pipeEnd = LoadTextureFromImage(pipeEnd);
    UnloadImage(pipeEnd);

    Image pipeStart = LoadImage("images/pipeStart.png");
    t.pipeStart = LoadTextureFromImage(pipeStart);
    UnloadImage(pipeStart);

    Image pipeIntersection = LoadImage("images/pipeIntersection.png");
    t.pipeIntersection = LoadTextureFromImage(pipeIntersection);
    UnloadImage(pipeIntersection);

    return t;
}

void unloadAllTextures(Textures& texture)
{
    UnloadTexture(texture.walkwayTile);
    UnloadTexture(texture.shieldTile);
    UnloadTexture(texture.thrusterOnTile);
    UnloadTexture(texture.thrusterOffTile);
}

void DrawGrid2D(Vector2 center, int slices, int spacing, Color color) {
    int halfSlices = slices / 2;

    rlBegin(RL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x + ((float)i * spacing), center.y + ((float)-halfSlices * spacing));
        rlVertex2f(center.x + ((float)i * spacing), center.y + ((float)halfSlices * spacing));

        rlVertex2f(center.x + ((float)-halfSlices * spacing), center.y + ((float)i * spacing));
        rlVertex2f(center.x + ((float)halfSlices * spacing), center.y + ((float)i * spacing));
    }
    rlEnd();
}

float wrap(float min, float max, float value)
{
    float range = max - min;
    return std::fmod(std::fmod(value - min, range) + range, range) + min;
}

bool TrackedGuiButton(std::vector<Rectangle>& uiRects, Rectangle rect, const char* label)
{
    uiRects.push_back(rect);
    return GuiButton(rect, label);
}

bool TrackedGuiDropdownBox(std::vector<Rectangle>& uiRects, Rectangle bounds,const char* text, int* active, bool editMode)
{
    if (editMode)
    {
        int itemCount = 1;
        for (const char* p = text; *p; p++)
            if (*p == ';') itemCount++;
        Rectangle expanded = { bounds.x, bounds.y,bounds.width, bounds.height * (itemCount + 1) };
        uiRects.push_back(expanded);
    }
    else
    {
        uiRects.push_back(bounds);
    }

    return GuiDropdownBox(bounds, text, active, editMode);
}

TileActionType getPipeType(std::unique_ptr<Pipe> pipes[BUILD_SIZE][BUILD_SIZE], Vector2 mousePos)
{
    int mask = 0;

    if (pipes[(int)mousePos.y][(int)mousePos.x]) mask |= UP;
    if (pipes[(int)mousePos.y][(int)mousePos.x]) mask |= RIGHT;
    if (pipes[(int)mousePos.y][(int)mousePos.x]) mask |= DOWN;
    if (pipes[(int)mousePos.y][(int)mousePos.x]) mask |= LEFT;

    switch (mask)
    {
    case 0b0000: return TileActionType::PipeStart;

    case 0b0001: return TileActionType::PipeEndUp;
    case 0b0010: return TileActionType::PipeEndRight;
    case 0b0100: return TileActionType::PipeEndDown;
    case 0b1000: return TileActionType::PipeEndLeft;

    case 0b0101: return TileActionType::PipeStraightUp;
    case 0b1010: return TileActionType::PipeStraightRight;

    case 0b0110: return TileActionType::PipeBentDR;
    case 0b1100: return TileActionType::PipeBentLD;
    case 0b1001: return TileActionType::PipeBentUL;
    case 0b0011: return TileActionType::PipeBentUR;

    case 0b0111: return TileActionType::PipeTURD;
    case 0b1110: return TileActionType::PipeTRDL;
    case 0b1101: return TileActionType::PipeDLU;
    case 0b1011: return TileActionType::PipeLUR;

    case 0b1111: return TileActionType::PipeIntersection;

    default: return TileActionType::PipeStart;
    }
}