#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include "Player.h"
#include "Enemy.h"
#include "UI.h"

// ================= LEVEL =================
class Level {
public:
    static constexpr int W = 10;
    static constexpr int H = 10;
    static constexpr float CELL = 2.0f;
    static constexpr float CEILING_Y = 3.0f;

    Model gunModel;
    Model ammoBoxModel;
    Model healBoxModel;

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

    Texture2D floorTex, wallTex, roofTex;
    Model cube;

    void Load()
    {
        // Textures
        floorTex = LoadTexture("assets/FloorTex.png");
        wallTex  = LoadTexture("assets/WallTex.png");
        roofTex  = LoadTexture("assets/RoofTex.png");
        cube = LoadModelFromMesh(GenMeshCube(1,1,1));

        // 3D Models
        ammoBoxModel = LoadModel("assets/ammobox.glb");
        healBoxModel = LoadModel("assets/healbox.glb");
    }

    bool IsWall(float x, float z) const
    {
        int gx = (int)floor(x / CELL);
        int gz = (int)floor(z / CELL);
        if(gx < 0 || gz < 0 || gx >= W || gz >= H) return true;
        return grid[gz][gx] == 1;
    }

    void Draw()
    {
        for(int z = 0; z < H; z++)
        for(int x = 0; x < W; x++)
        {
            Vector3 p = {x * CELL + CELL / 2, 0, z * CELL + CELL / 2};

            
            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTex;
            DrawModelEx(cube, {p.x, 0, p.z}, {0, 1, 0}, 0, {CELL, 0.02f, CELL}, WHITE);

            
            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = roofTex;
            DrawModelEx(cube, {p.x, CEILING_Y, p.z}, {0, 1, 0}, 0, {CELL, 0.02f, CELL}, WHITE);

            
            if(grid[z][x])
            {
                cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTex;
                DrawModelEx(cube, {p.x, CEILING_Y / 2, p.z}, {0, 1, 0}, 0, {CELL, CEILING_Y, CELL}, WHITE);
            }
            
            if (grid[z][x] == 2) 
            {
                DrawAmmoBox({p.x, 0.25f, p.z}, 0.2f, {0, 1, 0}, 45.0f); 
            }
            if (grid[z][x] == 3) 
            {
                DrawHealBox({p.x, 0.25f, p.z}, 0.2f, {1, 0, 0}, 30.0f, 0.4f); 
            }
        }
    }

    void DrawAmmoBox(Vector3 position, float scale, Vector3 rotationAxis, float rotationAngle)
    {
        DrawModelEx(ammoBoxModel, position, rotationAxis, rotationAngle, {scale, scale, scale}, WHITE);
    }

    void DrawHealBox(Vector3 position, float scale, Vector3 rotationAxis, float rotationAngle, float sphereSize)
    {
        
        DrawModelEx(healBoxModel, position, rotationAxis, rotationAngle, {scale, scale, scale}, WHITE);
        
       
        Vector3 spherePosition = {position.x, position.y + 0.5f, position.z};  
        DrawSphere(spherePosition, sphereSize, RED); 
    }
};

// ================= PICKUPS =================
struct HealthPack { Vector3 pos; float radius = 0.3f; bool active = true; };
struct AmmoPack   { Vector3 pos; float radius = 0.3f; int amount = 10; bool active = true; };

// ================= GAME =================
class Game {
public:
    Level level;

    std::vector<Turret> turrets = {
        {{2.51f, 0.25f, 17.38f}},
        {{5.46f, 0.25f, 17.38f}},
        {{11.08f, 0.25f, 11.07f}},
        {{14.91f, 0.25f, 16.83f}},
        {{17.05f, 0.25f, 11.24f}},
        {{11.43f, 0.25f, 3.64f}},
        {{14.82f, 0.25f, 3.64f}},
        {{17.06f, 0.25f, 3.64f}},
        {{11.45f, 0.25f, 6.61f}}
    };

    std::vector<Turret*> turretPtrs;

    Player player{&level, turretPtrs};

    std::vector<Projectile> projectiles;

    std::vector<HealthPack> healthPacks = {
        {{4.33f, 0.10f, 12.40f}},
        {{9.0f, 0.10f, 14.22f}},
        {{15.72f, 0.10f, 14.23f}},
        {{15.03f, 0.10f, 7.05f}}
    };

    std::vector<AmmoPack> ammoPacks = {
        {{5.89f, 0.0f, 14.24f}, 0.3f, 10, true},
        {{16.76f, 0.0f, 16.93f}, 0.3f, 10, true},
        {{15.07f, 0.0f, 9.10f}, 0.3f, 15, true}
    };

    Game()
    {
        for (auto& t : turrets)
            turretPtrs.push_back(&t);

        player.turrets = turretPtrs;
    }

    void Reset()
    {
        player.pos = {2.5f, 0, 2.5f};
        player.velY = 0;
        player.hp = player.maxHp;
        player.camHeight = player.standHeight;
        player.impacts.clear();
        player.lasers.clear();
        player.yaw = 0;
        player.pitch = 0;
        player.ammoInClip = player.clipSize;
        player.ammoStock = 30;

        for (auto& t : turrets)
        {
            t.hp = 30;
            t.fireCooldown = 0;
        }

        for (auto& pack : healthPacks)
            pack.active = true;
        for (auto& pack : ammoPacks)
            pack.active = true;

        projectiles.clear();
    }

    void CheckHealthPacks()
    {
        for (auto& pack : healthPacks)
        {
            if (!pack.active) continue;

            if (Vector3Distance(player.pos, pack.pos) < player.radius + pack.radius)
            {
                player.hp += 30;
                if (player.hp > player.maxHp) player.hp = player.maxHp;
                pack.active = false;
            }
        }
    }

    void CheckAmmoPacks()
    {
        for (auto& pack : ammoPacks)
        {
            if (!pack.active) continue;

            if (Vector3Distance(player.pos, pack.pos) < player.radius + pack.radius)
            {
                player.ammoStock += pack.amount;
                pack.active = false;
            }
        }
    }

    void DrawAmmoPacks()
    {
        for (auto& pack : ammoPacks)
            if (pack.active)
                level.DrawAmmoBox(pack.pos, 0.01f, {0, 1, 0}, 45.0f); 
    }

    void DrawHealPacks()
    {
        for (auto& pack : healthPacks)
            if (pack.active)
                level.DrawHealBox(pack.pos, 0.3f, {1, 0, 0}, -90.0f, 0.05f); 
    }

    void Run()
    {
        InitWindow(1280, 720, "DoomLike");
        DisableCursor();
        SetTargetFPS(60);


        player.gunModel = LoadModel("assets/gun.glb");
        
        level.Load();

        while (!WindowShouldClose())
        {
            float dt = GetFrameTime();

            player.Update(dt);

            for (auto& t : turrets)
                t.Update(dt, player.pos, projectiles, level);

            for (auto& p : projectiles)
                if (Vector3Distance(player.pos, p.pos) < player.radius + p.radius)
                {
                    player.hp -= 10;
                    player.damageFlash = 0.2f;
                    p.life = 0;
                }

            CheckHealthPacks();
            CheckAmmoPacks();

            if (player.hp <= 0)
                Reset();

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
                level.Draw();
                for (auto& t : turrets) t.Draw();
                DrawAmmoPacks(); 
                DrawHealPacks(); 
                player.DrawLasers();
                player.DrawImpacts();
                player.DrawGun();
                for (auto& p : projectiles)
                    DrawSphere(p.pos, p.radius, VIOLET);
            EndMode3D();

            DrawFPS(10, 10);
            player.DrawHpBar();
            player.DrawAmmoUI();
            DrawCrosshair(6, 2);
            DrawMinimap(*this);
            DrawPlayerCoordinates(player.pos);
            DrawDamageFlash(player);

            EndDrawing();
        }

        CloseWindow();
    }
};