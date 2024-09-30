#pragma once
#include "raylib.h"
#include "Obstacles.h"

class Boid
{
public:
    Vector3 position();
    int size;
    int velocity;
    float minimumDistance;
    float maxPerceiveDistance;
    float cohesionRadius;
    float maxSteer;

private:
    // Vector2 Separate(Boid[] others);
    // Vector2 Align(Boid[] others);
    // Vector2 Group(Boid[] others);
    // Vector2 AvoidObstacles(Obstacles[]);
};
