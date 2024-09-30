#pragma once
#include "raylib.h"
#include "Obstacles.h"

class Boid
{
public:
    Boid(int x, int y, int radius);
    ~Boid();
    
    Vector2 _position = Vector2{0, 0};
    int centerX = 1;
    int centerY = 1;
    int _radius = 1;
    
    int velocityX = 2;
    int velocityY = 2;
    
    float minimumDistance;
    float maxPerceiveDistance;
    float cohesionRadius;
    float maxSteer;
    void DrawBoid();
    void Update();

private:
    // Vector2 Separate(Boid[] others);
    // Vector2 Align(Boid[] others);
    // Vector2 Group(Boid[] others);
    Vector2 AvoidObstacles(Obstacles[]);
};
