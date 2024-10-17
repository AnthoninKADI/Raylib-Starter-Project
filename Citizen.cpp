#include "Citizen.h"

Citizen::Citizen(int startX, int startY, float size, Color color, const Node* dest)
    : position{(float)startX, (float)startY}, size(size), color(color), destination(dest) {}

void Citizen::Update(const std::vector<std::vector<Node>>& grid)
{
    if (destination == nullptr) return; // No destination set

    // Calculate direction towards the destination
    float directionX = destination->x - position.x;
    float directionY = destination->y - position.y;

    // Calculate distance to destination
    float distance = sqrt(directionX * directionX + directionY * directionY);
    
    // Normalize the direction vector
    if (distance > 0.0f)
    {
        directionX /= distance;
        directionY /= distance;

        // Move the citizen toward the destination
        position.x += directionX * speed;
        position.y += directionY * speed;

        // Check if the citizen has reached the destination
        if (distance < 1.0f)
        {
            position.x = (float)destination->x; // Snap to destination
            position.y = (float)destination->y; // Snap to destination
        }
    }
}

bool Citizen::CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid)
{
    return !grid[x][y].obstacle; // Citizens can only move to non-obstacle cells
}

void Citizen::Draw() const
{
    DrawCircleV({position.x * 50 + 25, position.y * 50 + 25}, size, color);
}
