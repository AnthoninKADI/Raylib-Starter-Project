#include "Citizen.h"

Citizen::Citizen(int startX, int startY, float size, Color color, const Node* destination)
    : position{(float)startX, (float)startY}, size(size), color(color), speed(0.02f) 
{
    this->destination = { (float)destination->x, (float)destination->y };
}

void Citizen::Update(const std::vector<std::vector<Node>>& grid)
{
    int currentX = (int)position.x;
    int currentY = (int)position.y;

    // Calculer le vecteur de direction vers la destination
    Vector2 direction = { destination.x - position.x, destination.y - position.y };
    float distanceToDestination = sqrt(direction.x * direction.x + direction.y * direction.y);

    // Normaliser la direction
    if (distanceToDestination > 0.0f)
    {
        direction.x /= distanceToDestination;
        direction.y /= distanceToDestination;

        // Déplacer le citoyen lentement vers la destination
        position.x += direction.x * speed;
        position.y += direction.y * speed;
    }
}

bool Citizen::CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid)
{
    return !grid[x][y].obstacle;  // Les citoyens ne peuvent se déplacer que sur les cases sans obstacle
}

void Citizen::Draw() const
{
    DrawCircleV({position.x * 50 + 25, position.y * 50 + 25}, size, color);
}
