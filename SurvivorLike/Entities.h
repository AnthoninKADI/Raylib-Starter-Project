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
