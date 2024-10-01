﻿#pragma once
#include "raylib.h"
#include "Obstacles.h"
#include <raymath.h>
#include <vector>

class Boid
{
public:
    Boid(int x, int y, int radius);
    ~Boid();
    
    Vector2 _position = Vector2{0, 0};
    int _radius = 1;
    
    Vector2 velocity = Vector2{3, 3}; // Vitesse sous forme de Vector2
    
    float minimumDistance = 1.0f;
    float maxPerceiveDistance = 1.0f;
    float cohesionRadius = 1.0f;
    float maxSteer = 1.0f;
    float setMaxSpeed = 5.0f;

    void DrawBoid();
    void Update(Obstacles obstacles[], int numObstacles);
    
    Vector2 AvoidObstacles(Obstacles obstacles[], int numObstacles);
    Vector2 Separate (Boid[]);
    Vector2 Align(Boid[]);
    Vector2 Group(Boid[]);
};
