#pragma once
#include "raylib.h"
#include "algorithm"
#include "Obstacles.h"
#include <cmath>   // Pour std::cos et std::sin
#include <cstdlib> // Pour rand() et RAND_MAX
#include <raymath.h>
#include <vector>
#include "Boid.h"

Boid::Boid(int x, int y, int radius, Color colorP, BoidType typeP)
    : _position{static_cast<float>(x), static_cast<float>(y)}, _radius(radius), color(colorP), category(typeP)
{
    switch (typeP)
    {
    case BoidType::Scissor:
        predator = BoidType::Rock;
        break;
    case BoidType::Rock:
        predator = BoidType::Paper;
        break;
    default:
        predator = BoidType::Scissor;
    }
}

Boid::~Boid() = default;

void Boid::DrawBoid() {
    DrawCircleV(_position, _radius, color);
}

float Boid::RandomAngle() {
    return static_cast<float>(rand()) / RAND_MAX * 2 * PI; 
}

Vector2 Boid::GetNewDirection(float currentAngle) {
    float newAngle;
    do {
        newAngle = RandomAngle(); 
    } while (fabs(newAngle - currentAngle) < PI / 4); 

    return Vector2{cos(newAngle) * maxSteer, sin(newAngle) * maxSteer};
}

void Boid::Update(std::vector<Boid>& flock, Obstacles obstacles[], int numObstacles, float PredParam, float PreyParam)
{
    Vector2 separation = {0, 0};
    Vector2 alignment = {0, 0};
    Vector2 cohesion = {0, 0};
    Vector2 PredParameter = {0, 0};
    Vector2 PreyParameter = {0, 0};
    int neighborCount = 0;
    
    for (const Boid& other : flock)
    {
        if (&other == this) continue;

        const Vector2 difference = Vector2Subtract(_position, other._position);
        const float distance = Vector2Length(difference);

        if (distance < minimumDistance && distance > 0)
        {
            const Vector2 normDiff = Vector2Normalize(difference);
            separation = Vector2Add(separation, Vector2Scale(normDiff, (minimumDistance - distance) * 0.1));
        }

        if (distance > 0 && distance < minimumDistance * 5)
        {
            alignment = Vector2Add(alignment, other.velocity);
            cohesion = Vector2Add(cohesion, other._position);
            neighborCount++;
        }

        if (other.category == predator && distance < minimumDistance * 3)
        {
            Vector2 predatorDiff = Vector2Normalize(difference);
            PredParameter = Vector2Add(PredParameter, Vector2Scale(predatorDiff, PredParam));
        }

        if (other.category != predator && other.category != category && distance < minimumDistance * 3)
        {
            Vector2 preyDiff = Vector2Normalize(Vector2Subtract(other._position, _position));
            PreyParameter = Vector2Add(PreyParameter, Vector2Scale(preyDiff, PreyParam));
        }
    }

    if (neighborCount > 0)
    {
        alignment = Vector2Scale(alignment, 1.0f / float(neighborCount));
        alignment = Vector2Scale(Vector2Normalize(alignment), 1.0f);

        cohesion = Vector2Scale(cohesion, 1.0f / float(neighborCount));
        const Vector2 cohesionForce = Vector2Subtract(cohesion, _position);
        cohesion = Vector2Scale(Vector2Normalize(cohesionForce), 0.1f);
    }
    
    _position.x += velocity.x;
    _position.y += velocity.y;

    if (_position.x < _radius || _position.x > GetScreenWidth() - _radius) {
        velocity.x *= -1;
        _position.x = Clamp(_position.x, _radius, GetScreenWidth() - _radius);
    }

    if (_position.y < _radius || _position.y > GetScreenHeight() - _radius) {
        velocity.y *= -1;
        _position.y = Clamp(_position.y, _radius, GetScreenHeight() - _radius);
    }

    for (int i = 0; i < numObstacles; i++) {
        if (_position.x + _radius > obstacles[i]._position.x &&
            _position.x - _radius < obstacles[i]._position.x + obstacles[i]._sizeX &&
            _position.y + _radius > obstacles[i]._position.y &&
            _position.y - _radius < obstacles[i]._position.y + obstacles[i]._sizeY) {

            Vector2 obstacleCenter = {
                obstacles[i]._position.x + obstacles[i]._sizeX / 2,
                obstacles[i]._position.y + obstacles[i]._sizeY / 2
            };

            Vector2 collisionNormal = Vector2Subtract(_position, obstacleCenter);
            collisionNormal = Vector2Normalize(collisionNormal);

            _position = Vector2Add(_position, Vector2Scale(collisionNormal, _radius));

            velocity = Vector2Reflect(velocity, collisionNormal); 
        }
    }

    velocity = Vector2Add(velocity, separation);
    velocity = Vector2Add(velocity, alignment);
    velocity = Vector2Add(velocity, cohesion);
    velocity = Vector2Add(velocity, PredParameter);
    velocity = Vector2Add(velocity, PreyParameter);
    velocity = Vector2Scale(Vector2Normalize(velocity), 5.0f);
}
