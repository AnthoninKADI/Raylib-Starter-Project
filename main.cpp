#include "raylib.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

#include "Obstacles.h"

using namespace std;

int main()
{
    cout << "Hello World" << endl;

    InitWindow(500, 500, "Inter AI MAS");
    SetTargetFPS(60);
    
    Obstacles a = Obstacles(45, 110, Vector2{75, 150});
    Obstacles b = Obstacles(75, 100, Vector2{250, 260});
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);
        a.DrawObstacle();
        b.DrawObstacle();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}