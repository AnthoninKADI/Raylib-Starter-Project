#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <algorithm>
#include <cmath>

void DrawCrosshair(int size, int thickness)
{
    int cx = GetScreenWidth()/2;
    int cy = GetScreenHeight()/2;

    DrawRectangle(cx-size-1, cy-thickness/2-1, size*2+2, thickness+2, BLACK);
    DrawRectangle(cx-thickness/2-1, cy-size-1, thickness+2, size*2+2, BLACK);
    DrawRectangle(cx-size, cy-thickness/2, size*2, thickness, WHITE);
    DrawRectangle(cx-thickness/2, cy-size, thickness, size*2, WHITE);
}

Model MakeTexturedCube(Texture2D tex)
{
    Mesh mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
    return model;
}

class Level {
public:
    static constexpr int W = 10;
    static constexpr int H = 10;
    static constexpr float CELL = 2.0f;

    int grid[H][W] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,1,0,0,0,0,1},
        {1,1,1,0,1,1,1,0,1,1},
        {1,0,0,0,0,0,1,0,0,1},
        {1,0,0,0,0,0,1,0,0,1},
        {1,0,0,0,0,0,1,0,0,1},
        {1,0,0,0,1,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1}
    };
};

struct Impact {
    Vector3 pos;
    float life;
};

class Player {
public:
    Camera3D cam;
    Vector3 pos = {2.5f, 0.0f, 2.5f};

    float yaw = 0.0f, pitch = 0.0f;
    float speed = 3.0f, radius = 0.3f;

    float bobPhase = 0.0f;
    float bobSpeed = 10.0f;
    float bobY = 0.05f;
    float bobX = 0.03f;

    float recoil = 0.0f;

    Level* level;
    std::vector<Impact> impacts;

    Player(Level* lvl): level(lvl)
    {
        cam.up = {0.0f,1.0f,0.0f};
        cam.fovy = 60.0f;
        cam.projection = CAMERA_PERSPECTIVE;
    }

    bool IsWall(float x, float z) const
    {
        int gx = (int)floor(x / Level::CELL);
        int gz = (int)floor(z / Level::CELL);

        if(gx<0 || gz<0 || gx>=Level::W || gz>=Level::H) return true;
        return level->grid[gz][gx] == 1;
    }

    void Update(float dt)
    {
        Vector2 m = GetMouseDelta();
        yaw   -= m.x * 0.003f;
        pitch -= m.y * 0.003f;
        pitch = Clamp(pitch, -1.2f, 1.2f);

        pitch += recoil;
        recoil = Lerp(recoil, 0.0f, dt * 15.0f);

        Vector3 f = {sinf(yaw), 0.0f, cosf(yaw)};
        Vector3 r = {-f.z, 0.0f, f.x};
        Vector3 mv = {0.0f, 0.0f, 0.0f};

        if(IsKeyDown(KEY_W)) mv = Vector3Add(mv, f);
        if(IsKeyDown(KEY_S)) mv = Vector3Subtract(mv, f);
        if(IsKeyDown(KEY_A)) mv = Vector3Subtract(mv, r);
        if(IsKeyDown(KEY_D)) mv = Vector3Add(mv, r);

        bool moving = Vector3Length(mv) > 0.01f;
        if(moving) mv = Vector3Normalize(mv);

        Vector3 vel = Vector3Scale(mv, speed * dt);
        Vector3 np = pos;

        if(!IsWall(pos.x + vel.x, pos.z)) np.x += vel.x;
        if(!IsWall(np.x, pos.z + vel.z)) np.z += vel.z;
        pos = np;

        if(moving) bobPhase += dt * bobSpeed;
        else bobPhase = Lerp(bobPhase, 0.0f, dt * 10.0f);

        float bx = moving ? sinf(bobPhase * 0.5f) * bobX : 0.0f;
        float by = moving ? sinf(bobPhase) * bobY : 0.0f;

        cam.position = {pos.x + bx, 1.0f + by, pos.z};

        Vector3 dir = {
            sinf(yaw) * cosf(pitch),
            sinf(pitch),
            cosf(yaw) * cosf(pitch)
        };
        cam.target = Vector3Add(cam.position, dir);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            Shoot();

        for(auto& i : impacts) i.life -= dt;
        impacts.erase(std::remove_if(impacts.begin(), impacts.end(),
            [](Impact& i){ return i.life <= 0.0f; }), impacts.end());
    }

    void Shoot()
    {
        Vector3 origin = cam.position;
        Vector3 dir = Vector3Normalize(Vector3Subtract(cam.target, cam.position));
        recoil = 0.002f;

        float bestDist = 10000.0f;
        Vector3 hitPos;
        bool hit = false;

        for(float d = 0.0f; d < 50.0f; d += 0.05f)
        {
            Vector3 p = Vector3Add(origin, Vector3Scale(dir, d));
            int gx = (int)floor(p.x / Level::CELL);
            int gz = (int)floor(p.z / Level::CELL);

            if(gx<0||gz<0||gx>=Level::W||gz>=Level::H) break;

            if(level->grid[gz][gx]==1){
                bestDist = d;
                hitPos = p;
                hit = true;
                break;
            }
        }

        if(dir.y < 0.0f){
            float t = (0.0f - origin.y) / dir.y;
            if(t > 0.0f && t < bestDist){
                bestDist = t;
                hitPos = Vector3Add(origin, Vector3Scale(dir, t));
                hit = true;
            }
        }

        if(dir.y > 0.0f){
            float t = (2.0f - origin.y) / dir.y;
            if(t > 0.0f && t < bestDist){
                bestDist = t;
                hitPos = Vector3Add(origin, Vector3Scale(dir, t));
                hit = true;
            }
        }

        if(hit)
            impacts.push_back({hitPos, 0.25f});
    }

    void DrawImpacts() const
    {
        rlDisableDepthTest();
        for(const auto& i : impacts)
            DrawSphere(i.pos, 0.08f, RED);
        rlEnableDepthTest();
    }

    void DrawGun() const
    {
        rlDisableDepthTest();

        rlPushMatrix();
        rlTranslatef(cam.position.x, cam.position.y, cam.position.z);
        rlRotatef(RAD2DEG*yaw, 0.0f,1.0f,0.0f);
        rlRotatef(RAD2DEG*-pitch, 1.0f,0.0f,0.0f);
        rlTranslatef(-0.45f,-0.25f,0.7f);

        DrawCube({0.0f,-0.05f,0.0f},0.15f,0.2f,0.2f,{220,190,160,255});
        DrawCube({0.0f,-0.15f,0.15f},0.1f,0.35f,0.15f,GRAY);
        DrawCube({0.0f,0.05f,0.5f},0.18f,0.12f,0.6f,GRAY);

        rlPopMatrix();
        rlEnableDepthTest();
    }
};

class Game {
public:
    Level level;
    Player player{&level};

    Texture2D FloorTex, WallTex, RoofTex;
    Model FloorModel, WallModel, RoofModel;

    void Run()
    {
        InitWindow(1440, 860, "DoomLike FPS");
        DisableCursor();
        SetTargetFPS(60);

        FloorTex = LoadTexture("assets/FloorTex.png");
        WallTex  = LoadTexture("assets/WallTex.png");
        RoofTex  = LoadTexture("assets/RoofTex.png");

        FloorModel = MakeTexturedCube(FloorTex);
        WallModel  = MakeTexturedCube(WallTex);
        RoofModel  = MakeTexturedCube(RoofTex);

        while(!WindowShouldClose())
        {
            player.Update(GetFrameTime());

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
                DrawModelEx(FloorModel,{10.0f,-0.02f,10.0f},{0.0f,1.0f,0.0f},0.0f,{20.0f,0.02f,20.0f},WHITE);
                DrawModelEx(RoofModel, {10.0f, 2.02f,10.0f},{0.0f,1.0f,0.0f},0.0f,{20.0f,0.02f,20.0f},WHITE);

                for(int z=0; z<Level::H; z++)
                    for(int x=0; x<Level::W; x++)
                        if(level.grid[z][x]==1)
                            DrawModelEx(WallModel,
                                {x*2.0f+1.0f,1.0f,z*2.0f+1.0f},
                                {0.0f,1.0f,0.0f},
                                0.0f,
                                {2.0f,2.0f,2.0f},
                                WHITE);

                player.DrawImpacts();
                player.DrawGun();
            EndMode3D();

            DrawCrosshair(6,2);
            DrawFPS(10,10);
            EndDrawing();
        }

        CloseWindow();
    }
};
