#include "raylib.h"
#include "Boid.h"
#include "Obstacles.h"
#include <iostream>

using namespace std;

int main()
{
    InitWindow(800, 800, "Inter AI MAS");
    SetTargetFPS(60);

    int Amount = 50;
    std::vector<Boid> flock;

    for (int i = 0; i < Amount; ++i)
    {
        float x = 1 + i;
        float y = 100;

        flock.emplace_back(x, y, 10, WHITE, BoidType::Rock);
        flock.emplace_back(x + 250, y + 50, 10, RED, BoidType::Scissor);
        flock.emplace_back(x + 50, y + 300, 10, PURPLE, BoidType::Paper);
    }

    
    
    Obstacles obstacle1 = Obstacles(45, 110, Vector2{175, 350});
    Obstacles obstacle2 = Obstacles(75, 100, Vector2{450, 460});
    
    
    Obstacles obstacles[] = { obstacle1, obstacle2 };
    int numObstacles = sizeof(obstacles) / sizeof(obstacles[0]);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);

        for (Boid& boid : flock)
        {
            boid.Update(flock, obstacles, numObstacles, 0.5f, 0.5f);
            boid.DrawBoid();
        }

        obstacle1.DrawObstacle();
        obstacle2.DrawObstacle();
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
