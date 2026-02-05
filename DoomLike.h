#pragma once
#include "raylib.h"
#include "raymath.h"

class Level {
public:
    static const int WIDTH = 10;
    static const int HEIGHT = 10;
    static constexpr float CELL_SIZE = 2.0f;
    int grid[HEIGHT][WIDTH];

    Level() {
        int map[HEIGHT][WIDTH] = {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,1,0,1,0,1,0,0,1},
            {1,0,1,0,1,0,1,0,0,1},
            {1,0,0,0,0,0,1,0,0,1},
            {1,0,1,1,1,0,1,0,0,1},
            {1,0,1,0,0,0,0,0,0,1},
            {1,0,1,0,1,1,1,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        };

        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++)
                grid[y][x] = map[y][x];
    }

    void Draw3D() const {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (grid[y][x] == 1) {
                    Vector3 pos = {x * CELL_SIZE, 1.0f, y * CELL_SIZE};
                    DrawCube(pos, CELL_SIZE, 2.0f, CELL_SIZE, DARKGRAY);
                    DrawCubeWires(pos, CELL_SIZE, 2.0f, CELL_SIZE, BLACK);
                }
            }
        }
    }
};

class Player {
public:
    Camera3D camera;
    float yaw = 0.0f;        
    float pitch = 0.0f;      
    float speed = 5.0f;

    Player(Vector3 startPos) {
        camera.position = startPos;
        camera.target = Vector3Add(startPos, {0, 0, 1});
        camera.up = {0, 1, 0};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }

    void Update(float dt) {
        
        Vector2 mouse = GetMouseDelta();
        float sens = 0.003f;
        yaw   -= mouse.x * sens;    
        pitch -= mouse.y * sens;    
        pitch = Clamp(pitch, -0.5f, 0.5f); //Lock looking up and down a bit
        
        Vector3 forward = { sinf(yaw), 0, cosf(yaw) }; 
        Vector3 right   = { forward.z, 0, -forward.x };
        
        Vector3 move = {0,0,0};
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Add(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Subtract(move, right);

        if (Vector3Length(move) > 0.001f)
            move = Vector3Normalize(move);

        camera.position = Vector3Add(camera.position, Vector3Scale(move, speed * dt));
        
        camera.target = Vector3Add(camera.position, {
            sinf(yaw) * cosf(pitch),
            sinf(pitch),
            cosf(yaw) * cosf(pitch)
        });
    }

    void Draw3D() const {
        DrawCube(camera.position, 0.3f, 0.3f, 0.3f, BLUE);
    }
};

class Game {
public:
    Level level;
    Player player = Player({1.5f, 1.0f, 1.5f}); 

    void Run() {
        InitWindow(800, 600, "DoomLike Project 5y");
        DisableCursor();
        SetTargetFPS(60);

        while (!WindowShouldClose()) {
            float dt = GetFrameTime();
            player.Update(dt);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.camera);
            DrawGrid(50, 1.0f);
            level.Draw3D();
            player.Draw3D();
            EndMode3D();

            DrawFPS(10, 10);
            EndDrawing();
        }

        EnableCursor();
        CloseWindow();
    }
};
