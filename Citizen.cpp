#include "Citizen.h"

Citizen::Citizen(int startX, int startY, float size, Color color, const std::vector<Node*>& destinations)
    : position{(float)startX, (float)startY}, size(size), color(color), speed(0.02f), currentDestinationIndex(0), destinations(destinations) 
{
    
}

void Citizen::Update(const std::vector<std::vector<Node>>& grid)
{
    if (currentDestinationIndex >= destinations.size())
        return; 

    Node* destination = destinations[currentDestinationIndex];
    Vector2 direction = { destination->x - position.x, destination->y - position.y };
    float distanceToDestination = sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distanceToDestination > 0.0f)
    {
        direction.x /= distanceToDestination;
        direction.y /= distanceToDestination;
        
        position.x += direction.x * speed;
        position.y += direction.y * speed;
    }
    
    if (distanceToDestination < speed) 
    {
        currentDestinationIndex++; 
        SetNewDestination(); 
    }
}

void Citizen::SetNewDestination()
{
    if (currentDestinationIndex < destinations.size())
    {
        Node* newDestination = destinations[currentDestinationIndex];
        
    }
}

bool Citizen::CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid)
{
    return !grid[x][y].obstacle;  
}

void Citizen::Draw() const
{
    DrawCircleV({position.x * 50 + 25, position.y * 50 + 25}, size, color);
}
