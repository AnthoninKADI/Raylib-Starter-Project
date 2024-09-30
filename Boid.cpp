#include "Boid.h"

Boid::Boid(int x, int y, int radius)
{
    centerX = x;
    centerY = y;
    _radius = radius;
}

Boid::~Boid()
{
    
}

void Boid::DrawBoid()
{
    DrawCircle(centerX, centerY, _radius, WHITE);
}

void Boid::Update()
{
    centerX += velocityX;
    centerY += velocityY;
}


