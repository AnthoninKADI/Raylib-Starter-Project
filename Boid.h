#pragma once
#include "raylib.h"
#include "Obstacles.h"
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

    void DrawBoid() const;
    void Update(Obstacles obstacles[], int numObstacles) const;
    
    void Parameter(const std::vector<Boid>& flock,
                const std::vector<Obstacles>& obstacles, 
                float minDistance,
                float alignmentFactor,
                float cohesionFactor, 
                const Vector2& boundsMin,
                const Vector2& boundsMax);
    
    Vector2 AvoidObstacles(Obstacles obstacles[], int numObstacles);
};
