//imported
#include <raylib.h>
#include <optional>
#include <algorithm>
#include <memory>
#include <iostream>
#include "raygui.h"
    
//my files
#include "myMathFuncs.h"
#include "vector2ops.h"
#include "SpaceShip.h"
#include "constants.h"
#include "helpers.h"

void handleInput(SpaceShip& ship, Vector2 worldMousePos, GameContext& gameContext, Textures textures)
{

    if (IsKeyPressed(KEY_B))
    {
        if (gameContext.gameState == GameState::BuildTiles or gameContext.gameState == GameState::BuildPipes)
        {
            gameContext.gameState = GameState::Play;
            gameContext.camera.target = ship.getLoc();
            ship.rebuildRotationVectors();
        }
        else
        {
            gameContext.gameState = GameState::BuildTiles;
            gameContext.camera.target = { 0,0 };
            gameContext.camera.zoom = 1.0f;
        }
    }

    //left mouse button click events
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (gameContext.gameState == GameState::BuildTiles or gameContext.gameState == GameState::BuildPipes)
        {
            Vector2 mousePos = GetMousePosition();
            bool hoveringUI = false;
            for (const Rectangle& rect : gameContext.uiRects)
                if (CheckCollisionPointRec(mousePos, rect))
                {
                    hoveringUI = true; break;
                }

            if (!hoveringUI)
            {
                ship.checkPlaceRemoveTile(worldMousePos, textures, gameContext);
            }
        }
    }

    //right mouse button click events
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        gameContext.camera.target -= GetMouseDelta() / gameContext.camera.zoom;
    }

    //makes sure the camera doesnt get too zoomed in or out
    gameContext.camera.zoom = std::clamp(gameContext.camera.zoom + GetMouseWheelMove() * 0.2f, 0.2f, 2.0f);

    //Rotating only prob
    if (IsKeyPressed(KEY_R))
    {
        if (gameContext.tileActionType == TileActionType::ThrusterUp) gameContext.tileActionType = TileActionType::ThrusterRight;
        else if (gameContext.tileActionType == TileActionType::ThrusterRight) gameContext.tileActionType = TileActionType::ThrusterDown;
        else if (gameContext.tileActionType == TileActionType::ThrusterDown) gameContext.tileActionType = TileActionType::ThrusterLeft;
        else if (gameContext.tileActionType == TileActionType::ThrusterLeft) gameContext.tileActionType = TileActionType::ThrusterUp;
    }
}

void updateLoop(SpaceShip& ship, float dt, GameContext& gameContext, Textures& textures)
{
    //what to do in each game state
    if (gameContext.gameState == GameState::Play)
    {
        ship.move(dt, textures);
    }
    else if (gameContext.gameState == GameState::BuildTiles or gameContext.gameState == GameState::BuildPipes)
    {

    }
}

void draw(SpaceShip& ship, GameContext& gameContext, Textures& textures)
{
    gameContext.uiRects.clear();
    BeginDrawing();
    BeginMode2D(gameContext.camera);
    ClearBackground(RAYWHITE);

    //render stuff that dis dynamic on camera
    if (gameContext.gameState == GameState::Play)
    {
        ship.drawWorldSpaceShip();
        
    }
    else if (gameContext.gameState == GameState::BuildTiles or gameContext.gameState == GameState::BuildPipes)
    {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), gameContext.camera);
        if (mousePos.x >= -624 and mousePos.x < 624 and mousePos.y >= -624 and mousePos.y < 624)
        {
            ship.drawTileOnMouse(mousePos, textures, gameContext);
        }
        ship.drawScreenSpaceShip(gameContext);
        DrawGrid2D({ 0,0 }, 50, 25, BLACK);
    }



    EndMode2D();
    
    //render stuff that is static on camera
    DrawFPS(10, 10);
    DrawText(gameStateToString(gameContext.gameState), 10, 40, 20, BLACK);
    if (gameContext.gameState == GameState::Play)
    {

    }
    else if (gameContext.gameState == GameState::BuildTiles)
    {
        // how much does an elephant weigh? A button.
        float buttonWidth = 80.0f;
        float buttonHeight = 40.0f;
        float spacing = 20.0f;

        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float startX = (GetScreenWidth() - totalWidth) / 2.0f;
        float y = (float)GetScreenHeight() - 50;

        if (TrackedGuiButton(gameContext.uiRects, { startX + 0 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Walkway"))
        {
            gameContext.tileActionType = TileActionType::Walkway;
        }

        if (TrackedGuiButton(gameContext.uiRects, { startX + 1 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Shield"))
        {
            gameContext.tileActionType = TileActionType::Shield;
        }

        if (TrackedGuiButton(gameContext.uiRects, { startX + 2 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Thruster"))
        {
            gameContext.tileActionType = TileActionType::ThrusterUp;
        }

        if (TrackedGuiButton(gameContext.uiRects, { startX + 3 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Remove"))
        {
            gameContext.tileActionType = TileActionType::Remove;
        }

        if (TrackedGuiDropdownBox(gameContext.uiRects, { (float)GetScreenWidth()-220, 20, 200, 30}, "Tiles;Pipes", &gameContext.buildDropDownActiveItem, gameContext.dropdownEditMode)) {
            gameContext.dropdownEditMode = !gameContext.dropdownEditMode;
            if (gameContext.buildDropDownActiveItem == 0)
            {
                gameContext.gameState = GameState::BuildTiles;
            }
            else if (gameContext.buildDropDownActiveItem == 1)
            {
                gameContext.gameState = GameState::BuildPipes;
            }
        }
    }
    else if (gameContext.gameState == GameState::BuildPipes)
    {
        // how much does an elephant weigh? A button.
        float buttonWidth = 80.0f;
        float buttonHeight = 40.0f;
        float spacing = 20.0f;

        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float startX = (GetScreenWidth() - totalWidth) / 2.0f;
        float y = (float)GetScreenHeight() - 50;

        if (TrackedGuiButton(gameContext.uiRects, { startX + 0 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Walkway"))
        {
            gameContext.tileActionType = TileActionType::Walkway;
        }

        if (TrackedGuiButton(gameContext.uiRects, { startX + 3 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Remove"))
        {
            gameContext.tileActionType = TileActionType::Remove;
        }

        if (TrackedGuiDropdownBox(gameContext.uiRects, { (float)GetScreenWidth() - 220, 20, 200, 30 }, "Tiles;Pipes", &gameContext.buildDropDownActiveItem, gameContext.dropdownEditMode)) {
            gameContext.dropdownEditMode = !gameContext.dropdownEditMode;
            if (gameContext.buildDropDownActiveItem == 0)
            {
                gameContext.gameState = GameState::BuildTiles;
            }
            else if (gameContext.buildDropDownActiveItem == 1)
            {
                gameContext.gameState = GameState::BuildPipes;
            }
        }
    }
    
    EndDrawing();
}

int main()
{
    //window setup 
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(900, 500, "cosmoteer rip off");
    SetTargetFPS(60);

    //ship typ shi
    //use a pointer instead so that the stack does not get discombobulated
    auto ship = std::make_unique<SpaceShip>();
    GameContext gameContext;
    //camera setup stuff
    gameContext.camera = { 0 };
    gameContext.camera.offset = { (float)GetScreenWidth() / 2,(float)GetScreenHeight() / 2};
    gameContext.camera.rotation = 0.0f;
    gameContext.camera.target = { 0,0 };
    gameContext.camera.zoom = 1.0f;

    //load all textures
    Textures textures = loadAllTextures();

    while (!WindowShouldClose())
    {
        //useful stuff
        Vector2 screenMousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(screenMousePos, gameContext.camera);
        float dt = GetFrameTime();
        handleInput(*ship, worldMousePos, gameContext, textures);
        updateLoop(*ship, dt, gameContext, textures);
        draw(*ship, gameContext, textures);
    }

    unloadAllTextures(textures);
    CloseWindow();
    return 0;
}