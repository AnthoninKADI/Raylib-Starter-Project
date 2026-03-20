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
        floorTex = LoadTexture("assets/FloorTex.png");
        wallTex  = LoadTexture("assets/WallTex.png");
        roofTex  = LoadTexture("assets/RoofTex.png");
        cube = LoadModelFromMesh(GenMeshCube(1,1,1));
    }

    bool IsWall(float x, float z) const
    {
        int gx = (int)floor(x/CELL);
        int gz = (int)floor(z/CELL);
        if(gx<0||gz<0||gx>=W||gz>=H) return true;
        return grid[gz][gx]==1;
    }

    void Draw()
    {
        for(int z=0;z<H;z++)
        for(int x=0;x<W;x++)
        {
            Vector3 p={x*CELL+CELL/2,0,z*CELL+CELL/2};

            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=floorTex;
            DrawModelEx(cube,{p.x,0,p.z},{0,1,0},0,{CELL,0.02f,CELL},WHITE);

            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=roofTex;
            DrawModelEx(cube,{p.x,CEILING_Y,p.z},{0,1,0},0,{CELL,0.02f,CELL},WHITE);

            if(grid[z][x])
            {
                cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=wallTex;
                DrawModelEx(cube,{p.x,CEILING_Y/2,p.z},{0,1,0},0,{CELL,CEILING_Y,CELL},WHITE);
            }
        }
    }
};

// ================= PICKUPS =================
struct HealthPack { Vector3 pos; float radius = 0.3f; bool active = true; };
struct AmmoPack   { Vector3 pos; float radius = 0.3f; int amount=10; bool active = true; };

// ================= GAME =================
class Game {
public:
    Level level;

    std::vector<Turret> turrets = {
        {{7.5f,0.25f,15.5f}},
        {{8.0f,0.25f,12.5f}},
        {{5.0f,0.25f,8.0f}}
    };

    std::vector<Turret*> turretPtrs;

    Player player{&level, turretPtrs};

    std::vector<Projectile> projectiles;

    std::vector<HealthPack> healthPacks = {
        {{3.5f,0.25f,3.5f}},
        {{6.5f,0.25f,10.5f}}
    };

    std::vector<AmmoPack> ammoPacks = {
        {{4.5f,0.25f,4.5f},0.3f,10,true},
        {{7.0f,0.25f,7.0f},0.3f,15,true}
    };

    Game()
    {
        for(auto& t : turrets)
            turretPtrs.push_back(&t);

        player.turrets = turretPtrs;
    }

    void Reset()
    {
        player.pos = {2.5f,0,2.5f};
        player.velY=0;
        player.hp=player.maxHp;
        player.camHeight=player.standHeight;
        player.impacts.clear();
        player.lasers.clear();
        player.yaw=0;
        player.pitch=0;
        player.ammoInClip=player.clipSize;
        player.ammoStock=30;

        for(auto& t: turrets)
        {
            t.hp=30;
            t.fireCooldown=0;
        }

        for(auto& pack: healthPacks) pack.active = true;
        for(auto& pack: ammoPacks) pack.active = true;

        projectiles.clear();
    }

    void CheckHealthPacks()
    {
        for(auto& pack : healthPacks)
        {
            if(!pack.active) continue;

            if(Vector3Distance(player.pos, pack.pos) < player.radius + pack.radius)
            {
                player.hp += 30;
                if(player.hp>player.maxHp) player.hp=player.maxHp;
                pack.active = false;
            }
        }
    }

    void CheckAmmoPacks()
    {
        for(auto& pack : ammoPacks)
        {
            if(!pack.active) continue;

            if(Vector3Distance(player.pos, pack.pos) < player.radius + pack.radius)
            {
                player.ammoStock += pack.amount;
                pack.active = false;
            }
        }
    }

    void DrawHealthPacks()
    {
        for(auto& pack: healthPacks)
            if(pack.active)
                DrawCube(pack.pos,0.4f,0.4f,0.4f,GREEN);
    }

    void DrawAmmoPacks()
    {
        for(auto& pack: ammoPacks)
            if(pack.active)
                DrawCube(pack.pos,0.4f,0.4f,0.4f,YELLOW);
    }

    void Run()
    {
        InitWindow(1280,720,"DoomLike");
        DisableCursor();
        SetTargetFPS(60);

        level.Load();

        while(!WindowShouldClose())
        {
            float dt=GetFrameTime();

            player.Update(dt);

            for(auto& t: turrets)
                t.Update(dt, player.pos, projectiles, level);

            for(auto&p : projectiles)
                if(Vector3Distance(player.pos,p.pos) < player.radius + p.radius) { 
                    player.hp -= 10; 
                    player.damageFlash = 0.2f;   
                    p.life = 0; 
                }

            CheckHealthPacks();
            CheckAmmoPacks();

            if(player.hp<=0)
                Reset();

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
                level.Draw();
                for(auto& t: turrets) t.Draw();
                DrawHealthPacks();
                DrawAmmoPacks();
                player.DrawLasers();
                player.DrawImpacts();
                player.DrawGun();
                for(auto&p:projectiles)
                    DrawSphere(p.pos,p.radius,VIOLET);
            EndMode3D();

            DrawFPS(10,10);
            player.DrawHpBar();
            player.DrawAmmoUI();
            DrawCrosshair(6,2);
            DrawMinimap(*this);
            DrawDamageFlash(player);

            EndDrawing();
        }

        CloseWindow();
    }
};