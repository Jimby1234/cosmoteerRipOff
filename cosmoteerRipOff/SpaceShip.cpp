#include "SpaceShip.h"
#include "raylib.h"
#include "constants.h"
#include "helpers.h"
#include "vector2ops.h"
#include <vector>
#include <unordered_set>
#include <queue>
#include <optional>
#include <iostream>

SpaceShip::SpaceShip() {
    position = { 0,0 };
    rotation = 0.0f;
    tileAmount = 0;
    angularVelocity = 0.0f;
    velocity = { 0.0f ,0.0f };
}

Vector2 SpaceShip::getLoc()
{
    return position;
}

void SpaceShip::drawWorldSpaceShip()
{
    float rad = rotation * DEG2RAD;

    Vector2 centre = calcCentreOfShip();

    for (size_t y = 0; y < std::size(tiles); ++y)
    {
        for (size_t x = 0; x < std::size(tiles[y]); ++x)
        {
            //skip if there is not a tile
            if (!tiles[y][x])
                continue;

            Tile& tile = *tiles[y][x];

            
            float tileLocalRotation = 0.0f;
            if (Thruster* thruster = dynamic_cast<Thruster*>(&tile))
            {
                tileLocalRotation = static_cast<float>(thruster->getRotation());
            }

            
            float localX = x * TILE_SIZE + TILE_SIZE / 2 - centre.x;
            float localY = y * TILE_SIZE + TILE_SIZE / 2 - centre.y;

            float rotatedX = localX * cosf(rad) - localY * sinf(rad);
            float rotatedY = localX * sinf(rad) + localY * cosf(rad);

            
            Vector2 origin = { TILE_SIZE / 2.0f, TILE_SIZE / 2.0f };

            
            Rectangle worldRect = {
                position.x + rotatedX + origin.x,
                position.y + rotatedY + origin.y,
                TILE_SIZE,
                TILE_SIZE
            };

            
            float totalRotation = rotation + tileLocalRotation;

            DrawTexturePro(tile.getTexture(), { 0,0,25,25 }, worldRect, origin, totalRotation, WHITE);
        }
    }
}

void SpaceShip::drawScreenSpaceShip()
{
    Vector2 centre = {
        (std::size(tiles[0]) * TILE_SIZE) / 2.0f,
        (std::size(tiles) * TILE_SIZE) / 2.0f
    };

    Vector2 origin = { TILE_SIZE / 2.0f, TILE_SIZE / 2.0f };

    for (size_t y = 0; y < std::size(tiles); ++y)
    {
        for (size_t x = 0; x < std::size(tiles[y]); ++x)
        {
            if (!tiles[y][x])
                continue;

            Tile& tile = *tiles[y][x];

            float tileLocalRotation = 0.0f;
            if (Thruster* thruster = dynamic_cast<Thruster*>(&tile))
                tileLocalRotation = static_cast<float>(thruster->getRotation());

            // Centre of this tile in screen space (offset so ship is centred)
            Rectangle rect = {
                x * TILE_SIZE - centre.x + origin.x,  // offset by origin so pivot is tile centre
                y * TILE_SIZE - centre.y + origin.y,
                TILE_SIZE,
                TILE_SIZE
            };

            Texture2D tex = tile.getTexture();
            Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };

            DrawTexturePro(tex, source, rect, origin, tileLocalRotation, WHITE);
        }
    }
}

void SpaceShip::checkPlaceRemoveTile(Vector2 mousePos, Textures texture, TileActionType tileActionType)
{
    Vector2 centre = { (std::size(tiles[0]) * TILE_SIZE) / 2.0f, (std::size(tiles) * TILE_SIZE) / 2.0f };

    int gridX = (int)floorf((mousePos.x + centre.x) / TILE_SIZE);
    int gridY = (int)floorf((mousePos.y + centre.y) / TILE_SIZE);

    bool inBounds = isInBounds(gridX, gridY);


    if (tileActionType == TileActionType::Remove)
    {
        //checks to see if the amount of tile connected in a certain direction is equal to the amount of all the tiles in the ship
        int amountOfTiles = countConnectedArea({ (float)gridX, (float)gridY });

        if (tiles[gridY][gridX] and inBounds and amountOfTiles == tileAmount)
        {
            tiles[gridY][gridX].reset();
            tileAmount -= 1;
        }
    }
    else
    {
        if ((!tiles[gridY][gridX] and hasNeighBour(gridX, gridY) and inBounds) or (!tiles[gridY][gridX] and tileAmount == 0 and inBounds))
        {
            if (tileActionType == TileActionType::Walkway) tiles[gridY][gridX] = std::make_unique<Tile>(texture.walkwayTile, 1.0f);
            else if (tileActionType == TileActionType::Shield) tiles[gridY][gridX] = std::make_unique<Tile>(texture.shieldTile, 5.0f);
            else if (tileActionType == TileActionType::ThrusterUp)
            {
                //                                               texture                 rot force  mass        
                tiles[gridY][gridX] = std::make_unique<Thruster>(texture.thrusterOffTile, 0, 11000.0f, 10.0f);
                upThrusters.push_back(Vector2{ (float)gridX, (float)gridY });
            }

            else if (tileActionType == TileActionType::ThrusterRight)
            {
                tiles[gridY][gridX] = std::make_unique<Thruster>(texture.thrusterOffTile, 90, 11000.0f, 10.0f);
                rightThrusters.push_back(Vector2{ (float)gridX, (float)gridY });
            }

            else if (tileActionType == TileActionType::ThrusterDown)
            {
                tiles[gridY][gridX] = std::make_unique<Thruster>(texture.thrusterOffTile, 180, 11000.0f, 10.0f);
                downThrusters.push_back(Vector2{ (float)gridX, (float)gridY });
            }

            else if (tileActionType == TileActionType::ThrusterLeft)
            {
                tiles[gridY][gridX] = std::make_unique<Thruster>(texture.thrusterOffTile, 270, 11000.0f, 10.0f);
                leftThrusters.push_back(Vector2{ (float)gridX, (float)gridY });
            }
            tileAmount += 1;
        }
    }
}

void SpaceShip::move(float dt, Textures texture)
{
    Vector2 totalForce = { 0, 0 };
    float totalTorque = 0;

    Vector2 centre = calcCentreOfShip();

    //move up/forward 
    if (IsKeyDown(KEY_W))
    {
        for (Vector2& thruster : upThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOnTile);
                applyThruster(thruster, centre, {0, -thrusta->getForce() }, totalForce, totalTorque);
            }
        }
    }
    //when W released turn thrusters off
    else if (IsKeyReleased(KEY_W))
    {
        for (Vector2& thruster : upThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }
    //move down/backwards
    if (IsKeyDown(KEY_S))
    {
        for (Vector2& thruster : downThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                applyThruster(thruster, centre, { 0, thrusta->getForce() }, totalForce, totalTorque);
                thrusta->setTexture(texture.thrusterOnTile);
            }
        }
    }
    //turn down thrusters off
    else if (IsKeyReleased(KEY_S))
    {
        for (Vector2& thruster : downThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }
    //move right
    if (IsKeyDown(KEY_E))
    {
        for (Vector2& thruster : rightThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                applyThruster(thruster, centre, { thrusta->getForce(), 0 }, totalForce, totalTorque);
                thrusta->setTexture(texture.thrusterOnTile);
            }
        }
    }
    //turn right thrusters off
    else if (IsKeyReleased(KEY_E))
    {
        for (Vector2& thruster : rightThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }
    //move left
    if (IsKeyDown(KEY_Q))
    {
        for (Vector2& thruster : leftThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                applyThruster(thruster, centre, { -thrusta->getForce(), 0 }, totalForce, totalTorque);
                thrusta->setTexture(texture.thrusterOnTile);
            }
        }
    }
    //turn left thrusters off
    else if (IsKeyReleased(KEY_Q))
    {
        for (Vector2& thruster : leftThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }
    //turn CCW
    if (IsKeyDown(KEY_A))
    {
        for (Vector2& thruster : CCWThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                applyThruster(thruster, centre, thrusterForceFromRotation(thrusta), totalForce, totalTorque);
                thrusta->setTexture(texture.thrusterOnTile);
            }
        }
    }
    //turn CCW thrusters off
    else if (IsKeyReleased(KEY_A))
    {
        for (Vector2& thruster : CCWThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }
    //turn CW
    if (IsKeyDown(KEY_D))
    {
        for (Vector2& thruster : CWThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                applyThruster(thruster, centre, thrusterForceFromRotation(thrusta), totalForce, totalTorque);
                thrusta->setTexture(texture.thrusterOnTile);
            }
        }
    }
    //turn CW thrusters off
    else if (IsKeyReleased(KEY_D))
    {
        for (Vector2& thruster : CWThrusters)
        {
            Tile& tile = *tiles[(int)thruster.y][(int)thruster.x];
            if (Thruster* thrusta = dynamic_cast<Thruster*>(&tile))
            {
                thrusta->setTexture(texture.thrusterOffTile);
            }
        }
    }

    float mass = getTotalMass();
    Vector2 accelLocal = totalForce / mass;

    float rad = rotation * DEG2RAD;

    Vector2 accelWorld = {
        accelLocal.x * cosf(rad) - accelLocal.y * sinf(rad),
        accelLocal.x * sinf(rad) + accelLocal.y * cosf(rad)
    };

    velocity += accelWorld * dt;
    position += velocity * dt;

    float inertia = getInertia() * TILE_SIZE * TILE_SIZE;
    float angularAccel = totalTorque / inertia;

    angularVelocity += angularAccel * RAD2DEG * dt;
    rotation += angularVelocity * dt;
}

bool SpaceShip::isInBounds(int x, int y)
{
    return x >= 0 and y >= 0 and x < std::size(tiles[y]) and y < std::size(tiles);
}

bool SpaceShip::hasNeighBour(int x, int y)
{
    bool check = false;
    if (isInBounds(x, y - 1)) check = check or tiles[y - 1][x];
    if (isInBounds(x + 1, y)) check = check or tiles[y][x + 1];
    if (isInBounds(x, y + 1)) check = check or tiles[y + 1][x];
    if (isInBounds(x - 1, y)) check = check or tiles[y][x - 1];

    return check;
}

Vector2 SpaceShip::calcCentreOfShip()
{
    if (tileAmount == 0)
    {
        return { (std::size(tiles[0]) * TILE_SIZE) / 2.0f, (std::size(tiles) * TILE_SIZE) / 2.0f };
    }

    float totalY = 0;
    float totalX = 0;
    int count = 0;

    for (int y = 0; y < std::size(tiles); y++)
    {
        for (int x = 0; x < std::size(tiles[y]); x++)
        {
            if (tiles[y][x])
            {
                Tile& tile = *tiles[y][x];
                totalY += y * tile.getMass();
                totalX += x * tile.getMass();
                count += tile.getMass();
            }
        }
    }
    float centreX = totalX / count * TILE_SIZE + TILE_SIZE / 2;
    float centreY = totalY / count * TILE_SIZE + TILE_SIZE / 2;
    return { centreX , centreY };
}

int SpaceShip::countConnectedArea(Vector2 start)
{
    Vector2 directions[4] = { {1,0}, {0,-1}, {-1,0}, {0,1} };

    // Find the first valid neighbour to use as seed
    Vector2 seed = { -1, -1 };
    for (Vector2 dir : directions)
    {
        Vector2 neighbour = start + dir;
        bool inBounds = neighbour.x >= 0 && neighbour.x < BUILD_SIZE
            && neighbour.y >= 0 && neighbour.y < BUILD_SIZE;
        if (inBounds && tiles[(int)neighbour.y][(int)neighbour.x])
        {
            seed = neighbour;
            break;
        }
    }

    if (seed.x == -1) return 0;

    std::unordered_set<Vector2> visited;
    std::queue<Vector2> toVisit;

    visited.insert(start);
    visited.insert(seed);
    toVisit.push(seed);

    while (!toVisit.empty())
    {
        Vector2 current = toVisit.front();
        toVisit.pop();
        for (Vector2 dir : directions)
        {
            Vector2 neighbour = current + dir;
            bool inBounds = neighbour.x >= 0 && neighbour.x < BUILD_SIZE
                && neighbour.y >= 0 && neighbour.y < BUILD_SIZE;
            if (visited.count(neighbour) == 0
                && inBounds
                && tiles[(int)neighbour.y][(int)neighbour.x])
            {
                visited.insert(neighbour);
                toVisit.push(neighbour);
            }
        }
    }

    return visited.size();
}

void SpaceShip::drawTileOnMouse(Vector2 mousePos, Textures texture, TileActionType tileActionType)
{
    int x = (int)floorf(mousePos.x / TILE_SIZE);
    int y = (int)floorf(mousePos.y / TILE_SIZE);

    Rectangle rect = {
        (float)(x * TILE_SIZE) + TILE_SIZE / 2.0f,
        (float)(y * TILE_SIZE) + TILE_SIZE / 2.0f,
        TILE_SIZE,
        TILE_SIZE
    };

    //{} to initialize memory wtf
    Texture2D tex{};
    float rotation{};
    if (tileActionType == TileActionType::Walkway)
    {
        tex = texture.walkwayTile;
        rotation = 0;
    }
    else if (tileActionType == TileActionType::Shield)
    {
        tex = texture.shieldTile;
        rotation = 0;
    }
    else if (tileActionType == TileActionType::ThrusterUp)
    {
        tex = texture.thrusterOffTile;
        rotation = 0;
    }
    else if (tileActionType == TileActionType::ThrusterRight)
    {
        tex = texture.thrusterOffTile;
        rotation = 90;
    }
    else if (tileActionType == TileActionType::ThrusterDown)
    {
        tex = texture.thrusterOffTile;
        rotation = 180;
    }
    else if (tileActionType == TileActionType::ThrusterLeft)
    {
        tex = texture.thrusterOffTile;
        rotation = 270;
    }  
    DrawTexturePro(tex, { 0,0,TILE_SIZE,TILE_SIZE}, rect, { TILE_SIZE / 2.0f, TILE_SIZE / 2.0f }, rotation, WHITE);
}

void SpaceShip::whichVectorToAppendTo(Vector2 grid, TileActionType tileActionType)
{
    Vector2 centre = calcCentreOfShip();

    // Subtract 0.5 to undo the TILE_SIZE/2 offset added in calcCentreOfShip,
    // aligning the centre with the average tile index rather than middle of that tile
    float centreGridX = centre.x / TILE_SIZE - 0.5f;
    float centreGridY = centre.y / TILE_SIZE - 0.5f;

    float dx = grid.x - centreGridX;
    float dy = grid.y - centreGridY;

    if (dx < 0 && dy < 0)          // top-left quadrant
    {
        switch (tileActionType)
        {
        case TileActionType::ThrusterUp:    CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterDown:  CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterRight: CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterLeft:  CCWThrusters.push_back(grid); break;
        default: break;
        }
    }
    else if (dx > 0 && dy < 0)     // top-right quadrant
    {
        switch (tileActionType)
        {
        case TileActionType::ThrusterUp:    CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterDown:  CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterRight: CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterLeft:  CCWThrusters.push_back(grid); break;
        default: break;
        }
    }
    else if (dx < 0 && dy > 0)     // bottom-left quadrant
    {
        switch (tileActionType)
        {
        case TileActionType::ThrusterUp:    CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterDown:  CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterRight: CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterLeft:  CWThrusters.push_back(grid);  break;
        default: break;
        }
    }
    else if (dx > 0 && dy > 0)     // bottom-right quadrant
    {
        switch (tileActionType)
        {
        case TileActionType::ThrusterUp:    CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterDown:  CWThrusters.push_back(grid);  break;
        case TileActionType::ThrusterRight: CCWThrusters.push_back(grid); break;
        case TileActionType::ThrusterLeft:  CWThrusters.push_back(grid);  break;
        default: break;
        }
    }
    // dx == 0 or dy == 0: on a centre axis, produces no torque, ignored
}

void SpaceShip::rebuildRotationVectors()
{
    CWThrusters.clear();
    CCWThrusters.clear();

    for (const Vector2& pos : upThrusters)
        whichVectorToAppendTo(pos, TileActionType::ThrusterUp);
    for (const Vector2& pos : downThrusters)
        whichVectorToAppendTo(pos, TileActionType::ThrusterDown);
    for (const Vector2& pos : rightThrusters)
        whichVectorToAppendTo(pos, TileActionType::ThrusterRight);
    for (const Vector2& pos : leftThrusters)
        whichVectorToAppendTo(pos, TileActionType::ThrusterLeft);
}

float SpaceShip::getTotalMass()
{
    float mass = 0;
    for (auto& y : tiles)
    {
        for (auto& tile : y)
        {
            if (tile != nullptr)
            {
                mass += tile->getMass();
            }
        }
    }
    return mass;
}

float SpaceShip::getInertia()
{
    Vector2 centre = calcCentreOfShip();
    float inertia = 0.0f;
    for (int y = 0; y < std::size(tiles); y++)
    {
        for (int x = 0; x < std::size(tiles[y]); x++)
        {
            if (tiles[y][x] == nullptr)
            {
                continue;
            }
            Vector2 difference = { centre.x / TILE_SIZE - x, centre.y / TILE_SIZE - y };
            float distance = difference.x * difference.x + difference.y * difference.y;
            float mass = tiles[y][x]->getMass();
            inertia += distance * mass;
        }
    }
    return inertia;
}

void SpaceShip::applyThruster(Vector2 grid, Vector2 centreOfMass, Vector2 thrusterForce, Vector2& totalForce, float& totalTorque)
{
    Vector2 thrusterCentre = { grid.x * TILE_SIZE + TILE_SIZE / 2.0f , grid.y * TILE_SIZE + TILE_SIZE / 2.0f };
    Vector2 distance = thrusterCentre - centreOfMass;

    totalForce += thrusterForce;

    totalTorque += distance.x * thrusterForce.y - distance.y * thrusterForce.x;
}

Vector2 SpaceShip::thrusterForceFromRotation(Thruster* t) {
    float f = t->getForce();
    switch (t->getRotation()) {
    case 0:   return { 0,  -f };  
    case 90:  return { f,   0 }; 
    case 180: return { 0,   f };  
    case 270: return { -f,  0 };  
    default:  return { 0,   0 };
    }
}