#include "Obstacles.h"
#include <raylib.h>

Obstacles::Obstacles(int sizex, int sizey, Vector2 position)
{
    _sizeX = sizex;
    _sizeY = sizey;
    _position = position;
}

Obstacles::~Obstacles() = default;

void Obstacles::DrawObstacle()
{
    DrawRectangle(static_cast<int>(_position.x), static_cast<int>(_position.y), _sizeX, _sizeY, BLACK);
}
