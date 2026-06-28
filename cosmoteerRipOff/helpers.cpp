#include "helpers.h"
#include <raylib.h>
#include "raygui.h"
#include <rlgl.h>
#include <cmath>
#include <vector>

const char* gameStateToString(GameState state) {
    switch (state) {
    case GameState::BuildTiles: return "build";
    case GameState::BuildPipes: return "build";
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

void buildUIRects(std::vector<Rectangle>& uiRects, GameState gameState)
{
    uiRects.clear();
    if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
    {
        float buttonWidth = 80.0f;
        float buttonHeight = 40.0f;
        float spacing = 20.0f;
        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float startX = (GetScreenWidth() - totalWidth) / 2.0f;
        float y = (float)GetScreenHeight() - 50;

        for (int i = 0; i < 4; i++)
            uiRects.push_back({ startX + i * (buttonWidth + spacing), y, buttonWidth, buttonHeight });
    }
}