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

void handleInput(SpaceShip& ship,Camera2D& camera,GameState& gameState, Vector2 worldMousePos, Textures& texture, TileActionType& tileType, const std::vector<Rectangle>& uiRects)
{

    if (IsKeyPressed(KEY_B))
    {
        if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
        {
            gameState = GameState::Play;
            camera.target = ship.getLoc();
            ship.rebuildRotationVectors();
        }
        else
        {
            gameState = GameState::BuildTiles;
            camera.target = { 0,0 };
            camera.zoom = 1.0f;
        }
    }

    //left mouse button click events
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
        {
            Vector2 mousePos = GetMousePosition();
            bool hoveringUI = false;
            for (const Rectangle& rect : uiRects)
                if (CheckCollisionPointRec(mousePos, rect))
                {
                    hoveringUI = true; break;
                }

            if (!hoveringUI)
            {
                ship.checkPlaceRemoveTile(worldMousePos, texture, tileType);
            }
        }
    }

    //right mouse button click events
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        camera.target -= GetMouseDelta() / camera.zoom;
    }

    //makes sure the camera doesnt get too zoomed in or out
    camera.zoom = std::clamp(camera.zoom + GetMouseWheelMove() * 0.2f, 0.2f, 2.0f);

    //Rotating only prob
    if (IsKeyPressed(KEY_R))
    {
        if (tileType == TileActionType::ThrusterUp) tileType = TileActionType::ThrusterRight;
        else if (tileType == TileActionType::ThrusterRight) tileType = TileActionType::ThrusterDown;
        else if (tileType == TileActionType::ThrusterDown) tileType = TileActionType::ThrusterLeft;
        else if (tileType == TileActionType::ThrusterLeft) tileType = TileActionType::ThrusterUp;
    }
}

void updateLoop(GameState& gameState, SpaceShip& ship, float dt, Textures texture)
{
    //what to do in each game state
    if (gameState == GameState::Play)
    {
        ship.move(dt, texture);
    }
    else if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
    {

    }
}

void draw(Camera2D& camera, GameState& gameState, SpaceShip& ship, Textures& texture, TileActionType& tileActionType, std::vector<Rectangle>& uiRects)
{
    uiRects.clear();
    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    //render stuff that dis dynamic on camera
    if (gameState == GameState::Play)
    {
        ship.drawWorldSpaceShip();
        
    }
    else if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
    {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
        if (mousePos.x >= -624 and mousePos.x < 624 and mousePos.y >= -624 and mousePos.y < 624)
        {
            ship.drawTileOnMouse(mousePos, texture, tileActionType);
        }
        ship.drawScreenSpaceShip();
        DrawGrid2D({ 0,0 }, 50, 25, BLACK);
    }



    EndMode2D();
    
    //render stuff that is static on camera
    DrawFPS(10, 10);
    DrawText(gameStateToString(gameState), 10, 40, 20, BLACK);
    if (gameState == GameState::Play)
    {

    }
    else if (gameState == GameState::BuildTiles or gameState == GameState::BuildPipes)
    {
        // how much does an elephant weigh? A button.
        float buttonWidth = 80.0f;
        float buttonHeight = 40.0f;
        float spacing = 20.0f;

        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float startX = (GetScreenWidth() - totalWidth) / 2.0f;
        float y = (float)GetScreenHeight() - 50;

        if (TrackedGuiButton(uiRects, { startX + 0 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Walkway"))
        {
            tileActionType = TileActionType::Walkway;
        }

        if (TrackedGuiButton(uiRects, { startX + 1 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Shield"))
        {
            tileActionType = TileActionType::Shield;
        }

        if (TrackedGuiButton(uiRects, { startX + 2 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Thruster"))
        {
            tileActionType = TileActionType::ThrusterUp;
        }

        if (TrackedGuiButton(uiRects, { startX + 3 * (buttonWidth + spacing), y, buttonWidth, buttonHeight }, "Remove"))
        {
            tileActionType = TileActionType::Remove;
        }
        //switch between placing pipes and tiles
        GuiDropdownBox({ 20, 60, 200, 30 }, "Option 1;Option 2;Option 3", 0, false);
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

    //camera setup stuff
    Camera2D camera = { 0 };
    camera.offset = { (float)GetScreenWidth() / 2,(float)GetScreenHeight() / 2};
    camera.rotation = 0.0f;
    camera.target = { 0,0 };
    camera.zoom = 1.0f;

    //game state stuff
    GameState gameState = GameState::BuildTiles;

    //load all textures
    Textures textures = loadAllTextures();

    //tile selected
    TileActionType tileType = TileActionType::Walkway;

    //rects for ui checking
    std::vector<Rectangle> uiRects;
    while (!WindowShouldClose())
    {
        //useful stuff
        Vector2 screenMousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(screenMousePos, camera);
        float dt = GetFrameTime();
        buildUIRects(uiRects, gameState);
        handleInput(*ship, camera, gameState, worldMousePos, textures, tileType, uiRects);
        updateLoop(gameState, *ship, dt, textures);
        draw(camera, gameState, *ship, textures, tileType, uiRects);
    }

    unloadAllTextures(textures);
    CloseWindow();
    return 0;
}