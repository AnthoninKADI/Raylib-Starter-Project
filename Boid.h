#pragma once
#include "raylib.h"
#include "Obstacles.h"
#include <raymath.h>
#include <vector>

enum class BoidType
{
    Rock,
    Paper,
    Scissor
};

class Boid
{
public:
    Boid(int x, int y, int radius, Color colorP, BoidType type);
    ~Boid();
    
    Vector2 _position = Vector2{0, 0};
    int _radius = 1;
    
    Vector2 velocity = Vector2{3, 3}; 

    
    float minimumDistance = 25.0f;
    float maxPerceiveDistance = 0.45f;
    float cohesionRadius = 0.1f;
    float maxSteer = 1.0f;
    float setMaxSpeed = 5.0f;
    float RandomAngle();
    Color color;
    BoidType category, predator;
    
    Vector2 GetNewDirection(float currentAngle);

    void DrawBoid();
    void Update(std::vector<Boid>& flock, Obstacles obstacles[], int numObstacles, float PredParam, float PrayParam);
    
    Vector2 AvoidObstacles(Obstacles obstacles[], int numObstacles);
    Vector2 Separate (Boid[]);
    Vector2 Align(Boid[]);
    Vector2 Group(Boid[]);
};


