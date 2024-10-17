#include "Citizen.h"

Citizen::Citizen(int startX, int startY, float size, Color color)
    : position{(float)startX, (float)startY}, size(size), color(color) {}

void Citizen::Update(const std::vector<std::vector<Node>>& grid)
{
    int currentX = (int)position.x;
    int currentY = (int)position.y;

    // Mouvement aléatoire simple sur les cellules "obstacle"
    int dx = GetRandomValue(-1, 1);
    int dy = GetRandomValue(-1, 1);

    int newX = currentX + dx;
    int newY = currentY + dy;

    // Vérification si la nouvelle position est valide et est un obstacle
    if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size())
    {
        if (CanMoveTo(newX, newY, grid))
        {
            position.x = (float)newX;
            position.y = (float)newY;
        }
    }
}

bool Citizen::CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid)
{
    return grid[x][y].obstacle;  // Les citoyens ne peuvent se déplacer que sur les cases avec "obstacle = true"
}

void Citizen::Draw() const
{
    DrawCircleV({position.x * 50 + 25, position.y * 50 + 25}, size, color);
}
