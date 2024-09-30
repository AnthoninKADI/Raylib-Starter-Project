#pragma once
#include "raylib.h"
#include "algorithm"
#include "Obstacles.h"
#include <cmath>   // Pour std::cos et std::sin
#include <cstdlib> // Pour rand() et RAND_MAX
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
    
    float minimumDistance;
    float maxPerceiveDistance;
    float cohesionRadius;
    float maxSteer;
    float setMaxSpeed = 5.0f;

    void DrawBoid();
    void Update(Obstacles obstacles[], int numObstacles);

private:
    Vector2 AvoidObstacles(Obstacles obstacles[], int numObstacles);
    float RandomAngle(); // Méthode pour générer un angle aléatoire
    Vector2 GetNewDirection(float currentAngle); // Méthode pour obtenir une nouvelle direction
};

Boid::Boid(int x, int y, int radius)
    : _position{static_cast<float>(x), static_cast<float>(y)}, _radius(radius), 
      minimumDistance(20.0f), maxPerceiveDistance(100.0f), 
      cohesionRadius(50.0f), maxSteer(5.0f) {}

Boid::~Boid() = default;

void Boid::DrawBoid() {
    DrawCircleV(_position, _radius, WHITE);
}

float Boid::RandomAngle() {
    return static_cast<float>(rand()) / RAND_MAX * 2 * PI; // Angle entre 0 et 2 * PI
}

Vector2 Boid::GetNewDirection(float currentAngle) {
    float newAngle;
    do {
        newAngle = RandomAngle(); // Générer un nouvel angle
    } while (fabs(newAngle - currentAngle) < PI / 4); // S'assurer que la nouvelle direction n'est pas trop proche de l'ancienne

    return Vector2{cos(newAngle) * maxSteer, sin(newAngle) * maxSteer};
}

void Boid::Update(Obstacles obstacles[], int numObstacles) {
    // Mise à jour de la position
    _position.x += velocity.x;
    _position.y += velocity.y;

    // Vérification des bords de la fenêtre
    if (_position.x < 0 || _position.x > GetScreenWidth() - _radius) {
        // Rebondir contre le bord de l'écran
        float angle = (velocity.x < 0) ? 0 : PI; // Détermine la direction opposée
        velocity = GetNewDirection(angle); // Nouvelle direction
        _position.x = Clamp(_position.x, 0, GetScreenWidth() - _radius); // Assurez-vous qu'il reste dans les limites
    }

    if (_position.y < 0 || _position.y > GetScreenHeight() - _radius) {
        // Rebondir contre le bord de l'écran
        float angle = (velocity.y < 0) ? -PI / 2 : PI / 2; // Détermine la direction opposée
        velocity = GetNewDirection(angle); // Nouvelle direction
        _position.y = Clamp(_position.y, 0, GetScreenHeight() - _radius); // Assurez-vous qu'il reste dans les limites
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

            // Inverser la direction du boid
            velocity.x += collisionNormal.x * maxSteer;
            velocity.y += collisionNormal.y * maxSteer;

            // Assurez-vous que le boid ne pénètre pas dans l'obstacle
            // Déplacez le boid à l'extérieur de l'obstacle
            float overlapX = (_position.x + _radius) - obstacles[i]._position.x;
            float overlapY = (_position.y + _radius) - obstacles[i]._position.y;

            // Ajustez la direction pour éviter d'aller dans la même direction
            velocity = GetNewDirection(atan2(velocity.y, velocity.x) + PI); // Nouvelle direction
        }
    }

    void Boid::Parameters(const std::vector<Boid>& flock, const std::vector<Obstacles>& obstacles, 
                    float minDistance, float alignmentFactor, float cohesionFactor, 
                    const Vector2& boundsMin, const Vector2& boundsMax);
    {
        Vector2 separation = {0, 0};
        Vector2 alignment = {0, 0};
        Vector2 cohesion = {0, 0};
        int neighborCount = 0;

        for (const auto& other : flock)
        {
            if (&other == this) continue; 

            Vector2 difference = Vector2Subtract(_position, other.position);
            float distance = Vector2Length(difference);

            if (distance < minimumDistance && distance > 0)
            {
                // Separation rule: move away from nearby boids
                Vector2 normDiff = Vector2Normalize(difference);
                separation = Vector2Add(separation, Vector2Scale(normDiff, (minimumDistance - distance) * 0.65f)); // Increased separation force
            }

            if (distance > 0 && distance < minimumDistance * 5)
            {
                // Alignment: Adjust velocity to match nearby boids
                alignment = Vector2Add(alignment, other.velocity);

                // Cohesion: Move towards the center of mass of nearby boids
                cohesion = Vector2Add(cohesion, other.position);
                neighborCount++;
            }
        }

        if (neighborCount > 0)
        {
            // Average the alignment and cohesion influences
            alignment = Vector2Scale(alignment, 1.0f / neighborCount);
            alignment = Vector2Scale(Vector2Normalize(alignment), maxPerceiveDistance);

            cohesion = Vector2Scale(cohesion, 1.0f / neighborCount);
            Vector2 cohesionForce = Vector2Subtract(cohesion, _position);
            cohesion = Vector2Scale(Vector2Normalize(cohesionForce), cohesionRadius);
        }

        // Add obstacle avoidance
        for (const auto& obstacle : obstacles)
        {
            // Check if the boid is colliding with the rectangular obstacle
            Rectangle obstacleRect = {obstacle.position.x, obstacle.position.y, static_cast<float>(obstacle.size_x), static_cast<float>(obstacle.size_y)};
        
            // Check for collision between boid (circle) and obstacle (rectangle)
            if (CheckCollisionCircleRec(_position, minimumDistance / 3.0f, obstacleRect))
            {
                // Calculate the vector from the boid to the closest point on the obstacle's boundary
                Vector2 obstacleCenter = {obstacle.position.x + obstacle.size_x / 2.0f, obstacle.position.y + obstacle.size_y / 2.0f};
                Vector2 obstacleVec = Vector2Subtract(_position, obstacleCenter);
            
                // Normalize the vector to create a repulsion force
                Vector2 normObstacleVec = Vector2Normalize(obstacleVec);
            
                // Calculate the repulsion strength based on proximity
                float repulsionStrength = 10;  // You can scale this value to adjust repulsion strength
                separation = Vector2Add(separation, Vector2Scale(normObstacleVec, repulsionStrength));
            }
        }

        // Update velocity based on the three rules and obstacle avoidance
        velocity = Vector2Add(velocity, separation);
        velocity = Vector2Add(velocity, alignment);
        velocity = Vector2Add(velocity, cohesion);
    }
}


