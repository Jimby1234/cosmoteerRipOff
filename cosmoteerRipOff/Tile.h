#pragma once
#include "raylib.h"

class Tile {
public:

    virtual ~Tile() = default;

    Tile(Texture2D tex, float kg)
    {
        texture = tex;
        mass = kg;
    }

    Texture2D getTexture() const
    {
        return texture;
    }

    float getMass()
    {
        return mass;
    }

protected:
    Texture2D texture;
    float mass;
};

class Thruster : public Tile{
public:
    Thruster(Texture2D tex, int rot, float N, float kg) : Tile(tex, kg){
        rotation = rot;
        force = N;
    }
    int getRotation()
    {
        return rotation;
    }
    float getForce()
    {
        return force;
    }
    void setTexture(Texture2D tex)
    {
        texture = tex;
    }

private:
    int rotation;
    float force;
};