#pragma once
#include "raylib.h"
#include "raymath.h"
#include <cmath>

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

        for (int z = 0; z < HEIGHT; z++)
            for (int x = 0; x < WIDTH; x++)
                grid[z][x] = map[z][x];
    }

    void Draw3D() const {
        for (int z = 0; z < HEIGHT; z++) {
            for (int x = 0; x < WIDTH; x++) {
                if (grid[z][x] == 1) {
                    Vector3 pos = {
                        x * CELL_SIZE + CELL_SIZE * 0.5f,
                        1.0f,
                        z * CELL_SIZE + CELL_SIZE * 0.5f
                    };

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
    Vector3 position;

    float yaw   = 0.0f;
    float pitch = 0.0f;

    float speed  = 5.0f;
    float radius = 0.3f;

    Level* level = nullptr;
    
    float bobPhase = 0.0f;
    float bobSpeed = 8.0f;
    float bobAmount = 0.05f;
    float bobSideAmount = 0.03f;

    Player(Vector3 startPos, Level* lvl) {
        position = startPos;
        level = lvl;

        camera.up = {0, 1, 0};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }

    void Update(float dt) {
        
        Vector2 mouse = GetMouseDelta();
        float sens = 0.003f;

        yaw   -= mouse.x * sens;
        pitch -= mouse.y * sens;
        pitch = Clamp(pitch, -1.2f, 1.2f);

        Vector3 forward = { sinf(yaw), 0, cosf(yaw) };
        Vector3 right   = { -forward.z, 0, forward.x };
        
        Vector3 move = {0,0,0};
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

        bool isMoving = Vector3Length(move) > 0.001f;
        if (isMoving)
            move = Vector3Normalize(move);

        Vector3 velocity = Vector3Scale(move, speed * dt);
        
        Vector3 newPos = position;

        if (!IsWall(position.x + velocity.x, position.z))
            newPos.x += velocity.x;

        if (!IsWall(newPos.x, position.z + velocity.z))
            newPos.z += velocity.z;

        position = newPos;
        
        if (isMoving)
            bobPhase += dt * bobSpeed;
        else
            bobPhase = 0.0f;

        float bobY = sinf(bobPhase) * bobAmount;
        float bobX = sinf(bobPhase * 0.5f) * bobSideAmount;
        
        Vector3 camBase = position;
        camBase.y += 1.0f;

        Vector3 rightOffset = Vector3Scale(right, bobX);
        camera.position = Vector3Add(camBase, Vector3Add(rightOffset, {0, bobY, 0}));

        camera.target = Vector3Add(camera.position, {
            sinf(yaw) * cosf(pitch),
            sinf(pitch),
            cosf(yaw) * cosf(pitch)
        });
    }

    bool IsWall(float x, float z) const {
        int minX = (int)floor((x - radius) / Level::CELL_SIZE);
        int maxX = (int)floor((x + radius) / Level::CELL_SIZE);
        int minZ = (int)floor((z - radius) / Level::CELL_SIZE);
        int maxZ = (int)floor((z + radius) / Level::CELL_SIZE);

        for (int gz = minZ; gz <= maxZ; gz++) {
            for (int gx = minX; gx <= maxX; gx++) {
                if (gx < 0 || gx >= Level::WIDTH ||
                    gz < 0 || gz >= Level::HEIGHT)
                    return true;

                if (level->grid[gz][gx] == 1)
                    return true;
            }
        }
        return false;
    }
};

class Game {
public:
    Level level;
    Player player = Player({2.5f, 0.0f, 2.5f}, &level);

    void Run() {
        InitWindow(800, 600, "DoomLike - Stable Collisions + Old Bobbing");
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
            EndMode3D();

            DrawFPS(10, 10);
            EndDrawing();
        }

        EnableCursor();
        CloseWindow();
    }
};
