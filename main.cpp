#include "raylib.h"
#include <iostream>

using namespace std;

int main() {

    InitWindow(300, 300, "Compute Shaders");
    
     Camera3D camera = { 0 };
        camera.position = (Vector3{ 10.0f, 6.0f, 10.0f }); 
        camera.target = (Vector3{ 0.0f, 0.0f, 0.0f });      
        camera.up = (Vector3{ 0.0f, 1.0f, 0.0f });          
        camera.fovy = 45.0f;                                
        camera.projection = CAMERA_PERSPECTIVE;             
    
        Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
    
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
            BeginMode3D(camera);
                DrawCube(cubePosition, 6.0f, 0.1f, 6.0f, SKYBLUE);
                DrawCubeWires(cubePosition, 6.0f, 0.1f, 6.0f, BLACK);
                DrawGrid(10, 1.0f);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}