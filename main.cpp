#include "raylib.h"
#include "Boid.h"
#include "Obstacles.h"
#include <iostream>

using namespace std;

int main()
{
    InitWindow(500, 500, "Inter AI MAS");
    SetTargetFPS(60);

    int Amount = 100;
    std::vector<Boid> flock;

    for (int i = 0; i < Amount; ++i)
    {
        float x = 1 + i;
        float y = 100;

        flock.emplace_back(x, y, 5);
    }
    
    Obstacles obstacle1 = Obstacles(45, 110, Vector2{75, 150});
    Obstacles obstacle2 = Obstacles(75, 100, Vector2{250, 260});
    
    
    Obstacles obstacles[] = { obstacle1, obstacle2 };
    int numObstacles = sizeof(obstacles) / sizeof(obstacles[0]);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);

        for (Boid& boid : flock)
        {
            boid.Update(obstacles, numObstacles);
            boid.DrawBoid();
        }

        obstacle1.DrawObstacle();
        obstacle2.DrawObstacle();
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
