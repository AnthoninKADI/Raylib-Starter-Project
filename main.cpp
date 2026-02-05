#include "raylib.h"
#include "raymath.h"
#include <cmath>

int main()
{
    InitWindow(1280, 720, "Doom-like FPS");
    DisableCursor();
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = { 2.0f, 1.8f, 2.0f };
    camera.target   = { 0.0f, 1.8f, 0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 75.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float yaw = 0.0f;
    float pitch = 0.0f;
    const float mouseSensitivity = 0.003f;
    const float moveSpeed = 5.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Vector2 mouse = GetMouseDelta();
        yaw   -= mouse.x * mouseSensitivity;
        pitch -= mouse.y * mouseSensitivity;
        pitch = Clamp(pitch, -1.5f, 1.5f);
        
        Vector3 forward = {
            cosf(pitch) * sinf(yaw),
            sinf(pitch),
            cosf(pitch) * cosf(yaw)
        };

        camera.target = Vector3Add(camera.position, forward);
        Vector3 right = { sinf(yaw - PI/2), 0, cosf(yaw - PI/2) };
        
        if (IsKeyDown(KEY_W)) camera.position = Vector3Add(camera.position, Vector3Scale(forward, moveSpeed * dt));
        if (IsKeyDown(KEY_S)) camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, moveSpeed * dt));
        if (IsKeyDown(KEY_A)) camera.position = Vector3Subtract(camera.position, Vector3Scale(right, moveSpeed * dt));
        if (IsKeyDown(KEY_D)) camera.position = Vector3Add(camera.position, Vector3Scale(right, moveSpeed * dt));

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        DrawPlane({ 0, 0, 0 }, { 50, 50 }, GRAY);
        DrawCube({ 0, 1, 0 }, 1, 2, 1, BLUE); 
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
