#include "raylib.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

#include "Boid.h"
#include "Obstacles.h"

using namespace std;

int main()
{
    InitWindow(500, 500, "Inter AI MAS");
    SetTargetFPS(60);
    
    Obstacles a = Obstacles(45, 110, Vector2{75, 150});
    Obstacles b = Obstacles(75, 100, Vector2{250, 260});
    Boid c = Boid(50, 50, 7);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);
        c.Update();
        a.DrawObstacle();
        b.DrawObstacle();
        c.DrawBoid();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}