#pragma once
#include "raylib.h"

struct Enemy
{
    Vector2 pos;
    float speed;
    float size;
    Texture2D texture;
};

struct XPOrb
{
    Vector2 pos;
    float size;
    int value;
    Texture2D texture;
};

struct Projectile
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    bool active;
    float range;      
    float travelled;  
};

