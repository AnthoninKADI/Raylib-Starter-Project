#pragma once
#include "raylib.h"

class Obstacles
{
public:
    Obstacles (int sizex, int sizey, Vector2 position);
    ~Obstacles();
    
    Vector2 _position = Vector2{0, 0};
    int _sizeX = 1;
    int _sizey = 1;
    
    void DrawObstacle();
};
