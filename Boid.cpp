#pragma once
#include "raylib.h"
#include "algorithm"
#include "Obstacles.h"
#include <cmath>   // Pour std::cos et std::sin
#include <cstdlib> // Pour rand() et RAND_MAX
#include <raymath.h>
#include <vector>
#include "Boid.h"

Boid::Boid(int x, int y, int radius)
    : _position{static_cast<float>(x), static_cast<float>(y)}, _radius(radius) 
      {}

Boid::~Boid() = default;

void Boid::DrawBoid() {
    DrawCircleV(_position, _radius, WHITE);
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

void Boid::Update(std::vector<Boid>& flock, Obstacles obstacles[], int numObstacles)
{
Vector2 separation = {0, 0};
Vector2 alignment = {0, 0};
Vector2 cohesion = {0, 0};
int neighborCount = 0;
    
for (const Boid& other : flock)
{
    if (&other == this) continue;

    const Vector2 difference = Vector2Subtract(_position, other._position);
    const float distance = Vector2Length(difference);

    if (distance < minimumDistance && distance > 0)
    {
        // Separation
        const Vector2 normDiff = Vector2Normalize(difference);
        separation = Vector2Add(separation, Vector2Scale(normDiff, (minimumDistance - distance) * 0.1));
    }

    if (distance > 0 && distance < minimumDistance * 5)
    {
        // Allignment
        alignment = Vector2Add(alignment, other.velocity);

        // Cohesion
        cohesion = Vector2Add(cohesion, other._position);
        neighborCount++;
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
    
    // Mise à jour de la position
    _position.x += velocity.x;
    _position.y += velocity.y;

    // Vérification des bords de la fenêtre
    if (_position.x < (_radius * 10)|| _position.x > GetScreenWidth() - ( 10 * _radius)) {
        // Rebondir contre le bord de l'écran
        float direction = (_position.x <  (_radius * 10)) ? 1 : -1; 
        //velocity = GetNewDirection(angle); 
        velocity.x += 3.f * direction; 
        //_position.x = Clamp(_position.x, 0, GetScreenWidth() - _radius); 
    }

    if (_position.y < (_radius * 10) || _position.y > GetScreenHeight() - (_radius * 10)) {
        // Rebondir contre le bord de l'écran
        //float angle = (velocity.y < 0) ? -PI / 2 : PI / 2; 
        //velocity = GetNewDirection(angle);
        velocity.y += 3.f * (_position.y <  (_radius * 10) ? 1 : -1); 
        //_position.y = Clamp(_position.y, 0, GetScreenHeight() - _radius); 
    }

    // Vérification des collisions avec les obstacles
    for (int i = 0; i < numObstacles; i++) {
        if (_position.x + _radius > obstacles[i]._position.x &&
            _position.x - _radius < obstacles[i]._position.x + obstacles[i]._sizeX &&
            _position.y + _radius > obstacles[i]._position.y &&
            _position.y - _radius < obstacles[i]._position.y + obstacles[i]._sizeY) {

            // Collision détectée
            // Calculez le centre de l'obstacle
            Vector2 obstacleCenter = {
                obstacles[i]._position.x + obstacles[i]._sizeX / 2,
                obstacles[i]._position.y + obstacles[i]._sizeY / 2
            };

            // Calculez la direction de l'obstacle par rapport au boid
            Vector2 collisionNormal = {
                _position.x - obstacleCenter.x,
                _position.y - obstacleCenter.y
            };

            // Normalisez la direction de collision
            float length = sqrt(collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y);
            collisionNormal = {
                collisionNormal.x / length,
                collisionNormal.y / length
            };

            // Assurez-vous que le boid ne pénètre pas dans l'obstacle
            // Déplacez le boid à l'extérieur de l'obstacle
            float overlapX = (_position.x + _radius) - obstacles[i]._position.x;
            float overlapY = (_position.y + _radius) - obstacles[i]._position.y;

            // Ajustez la direction pour éviter d'aller dans la même direction
            velocity = GetNewDirection(atan2(velocity.y, velocity.x) + PI); // Nouvelle direction
        
            
        }
    }

      velocity = Vector2Add(velocity, separation);
      velocity = Vector2Add(velocity, alignment);
      velocity = Vector2Add(velocity, cohesion);
    velocity = Vector2Scale(Vector2Normalize(velocity), 5.0f);
}


