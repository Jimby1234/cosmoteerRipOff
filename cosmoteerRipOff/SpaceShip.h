#pragma once
#include "raylib.h"
#include "Tile.h"
#include "constants.h"
#include "helpers.h"
#include <optional>
#include <memory>
#include <vector>

class SpaceShip
{
public:

    SpaceShip();

    Vector2 getLoc();

    void drawWorldSpaceShip();

    void drawScreenSpaceShip(GameContext& gameContext);

    void checkPlaceRemoveTile(Vector2& mousePos, Textures& texture, GameContext& gameContext);

    void move(float dt, Textures texture);

    void drawTileOnMouse(Vector2& mousePos, Textures& texture, GameContext gameContext);

    void rebuildRotationVectors();

    float getTotalMass();

    float getInertia();

private:
    bool isInBounds(int x, int y);
    bool hasNeighBour(int x, int y);
    int countConnectedArea(Vector2 start);
    void whichVectorToAppendTo(Vector2 grid, TileActionType tileActionType);
    void applyThruster(Vector2 grid, Vector2 centreOfMass, Vector2 thrusterForce, Vector2& totalForce, float& totalTorque);
    Vector2 calcCentreOfShip();
    Vector2 thrusterForceFromRotation(Thruster* t);
    Vector2 position;
    float rotation;
    int tileAmount;
    std::unique_ptr<Tile> tiles[BUILD_SIZE][BUILD_SIZE];
    std::unique_ptr<Pipe> pipes[BUILD_SIZE][BUILD_SIZE];
    std::vector<Vector2> upThrusters;
    std::vector<Vector2> rightThrusters;
    std::vector<Vector2> downThrusters;
    std::vector<Vector2> leftThrusters;
    std::vector<Vector2> CWThrusters;
    std::vector<Vector2> CCWThrusters;
    Vector2 velocity;
    float angularVelocity;
    float inertia;
    Vector2 centre;
};