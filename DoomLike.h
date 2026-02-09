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
    DrawRectangle(cx-size, cy-thickness/2, size*2, thickness, WHITE);
    DrawRectangle(cx-thickness/2, cy-size, thickness, size*2, WHITE);
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

    Texture2D floorTex, wallTex, roofTex;
    Model cube;

    void Load()
    {
        floorTex = LoadTexture("assets/FloorTex.png");
        wallTex  = LoadTexture("assets/WallTex.png");
        roofTex  = LoadTexture("assets/RoofTex.png");
        cube = LoadModelFromMesh(GenMeshCube(1,1,1));
    }

    bool IsWallCell(int gx, int gz) const
    {
        if (gx < 0 || gz < 0 || gx >= W || gz >= H) return true;
        return grid[gz][gx] == 1;
    }

    bool IsWall(float x, float z) const
    {
        return IsWallCell((int)floor(x/CELL),(int)floor(z/CELL));
    }

    void Draw()
    {
        for(int z=0; z<H; z++)
        for(int x=0; x<W; x++)
        {
            Vector3 p = {x*CELL + CELL/2, 0, z*CELL + CELL/2};

            // floor
            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTex;
            DrawModelEx(cube, {p.x,0,p.z},{0,1,0},0,{CELL,0.02f,CELL},WHITE);

            // roof
            cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = roofTex;
            DrawModelEx(cube, {p.x,2,p.z},{0,1,0},0,{CELL,0.02f,CELL},WHITE);

            // walls
            if(grid[z][x])
            {
                cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTex;
                DrawModelEx(cube, {p.x,1,p.z},{0,1,0},0,{CELL,2,CELL},WHITE);
            }
        }
    }
};

struct Impact {
    Vector3 pos;
    float life;
};

struct Projectile {
    Vector3 pos;
    Vector3 dir;
    float life;
    float speed;
    float radius = 0.1f;
};

class Player {
public:
    Camera3D cam;
    Vector3 pos = {2.5f,0,2.5f};
    float yaw=0, pitch=0;
    float speed=3.0f;
    float radius=0.3f;
    float hp=100;
    float damageCooldown = 0;   
    float bobPhase=0;
    float bobSpeed=10;
    float bobX=0.03f;
    float bobY=0.05f;

    Level* level;
    std::vector<Impact> impacts;

    Player(Level* lvl):level(lvl)
    {
        cam.up={0,1,0};
        cam.fovy=60;
        cam.projection=CAMERA_PERSPECTIVE;
    }

    bool Collides(float x, float z) const
    {
        float r = radius + 0.01f;
        return level->IsWall(x-r,z-r)||level->IsWall(x+r,z-r)||
               level->IsWall(x-r,z+r)||level->IsWall(x+r,z+r);
    }

    void TakeDamage(float dmg)
    {
        if(damageCooldown <= 0) {
            hp -= dmg;
            damageCooldown = 0.5f; 
        }
    }

    void Update(float dt)
    {
        if(damageCooldown > 0) damageCooldown -= dt;

        Vector2 m=GetMouseDelta();
        yaw-=m.x*0.003f;
        pitch-=m.y*0.003f;
        pitch=Clamp(pitch,-1.2f,1.2f);

        Vector3 f={sinf(yaw),0,cosf(yaw)};
        Vector3 r={-f.z,0,f.x};
        Vector3 mv={0};

        if(IsKeyDown(KEY_W)) mv=Vector3Add(mv,f);
        if(IsKeyDown(KEY_S)) mv=Vector3Subtract(mv,f);
        if(IsKeyDown(KEY_A)) mv=Vector3Subtract(mv,r);
        if(IsKeyDown(KEY_D)) mv=Vector3Add(mv,r);

        bool moving = Vector3Length(mv)>0.01f;
        if(moving) mv=Vector3Normalize(mv);

        Vector3 step=Vector3Scale(mv,speed*dt);

        if(!Collides(pos.x+step.x,pos.z)) pos.x+=step.x;
        if(!Collides(pos.x,pos.z+step.z)) pos.z+=step.z;
        if(moving) bobPhase+=dt*bobSpeed;
        else bobPhase=0;
        
        float bx=sinf(bobPhase*0.5f)*bobX;
        float by=sinf(bobPhase)*bobY;

        cam.position={pos.x+bx,1.0f+by,pos.z};
        cam.target=Vector3Add(cam.position,{
            sinf(yaw)*cosf(pitch),
            sinf(pitch),
            cosf(yaw)*cosf(pitch)
        });

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            Shoot();

        for(auto&i:impacts) i.life-=dt;
        impacts.erase(std::remove_if(impacts.begin(),impacts.end(),
            [](auto&i){return i.life<=0;}),impacts.end());
    }

    void Shoot()
    {
        Vector3 dir=Vector3Normalize(Vector3Subtract(cam.target,cam.position));

        for(float d=0; d<50; d+=0.05f)
        {
            Vector3 p=Vector3Add(cam.position,Vector3Scale(dir,d));

            if(p.y<=0 || p.y>=2 || level->IsWall(p.x,p.z))
            {
                impacts.push_back({p,0.25f});
                break;
            }
        }
    }

    void DrawGun()
    {
        Vector3 gunSize = {0.15f, 0.2f, 0.4f};
        Vector3 gunOffset = {0.0f, -0.25f, 0.6f}; 

        rlPushMatrix();
        rlTranslatef(cam.position.x, cam.position.y, cam.position.z);
        rlRotatef(RAD2DEG * yaw, 0, 1, 0);
        rlRotatef(RAD2DEG * -pitch, 1, 0, 0);
        rlTranslatef(gunOffset.x, gunOffset.y, gunOffset.z);
        DrawCube({0,0,0}, gunSize.x, gunSize.y, gunSize.z, DARKGRAY);
        rlPopMatrix();
    }

    void DrawImpacts()
    {
        for(auto&i:impacts)
            DrawSphere(i.pos,0.08f,RED);
    }
};

class Turret {
public:
    Vector3 pos;
    float radius = 0.3f;
    float fireCooldown = 0;
    float fireRate = 2.0f; 
    float projSpeed = 6.0f;

    Turret(Vector3 p):pos(p){}

    void Update(float dt, Player& player, std::vector<Projectile>& projectiles, Level& level)
    {
        if(fireCooldown>0) fireCooldown-=dt;

        if(fireCooldown<=0)
        {
            Vector3 dir = Vector3Normalize(Vector3Subtract(player.pos, pos));
            projectiles.push_back({pos,dir,3.0f,projSpeed,0.1f});
            fireCooldown = fireRate;
        }
        
        for(auto& p:projectiles)
        {
            Vector3 np = Vector3Add(p.pos, Vector3Scale(p.dir,p.speed*dt));
            if(level.IsWall(np.x,np.z) || np.y<=0 || np.y>=2)
            {
                p.life=0;
            }
            else
            {
                p.pos=np;
            }
        }
        projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),
            [](auto&p){return p.life<=0;}),projectiles.end());
    }

    void Draw() const
    {
        DrawCube(pos,0.3f,1.5f,0.5f,BLUE);
    }
};

class Game {
public:
    Level level;
    Player player{&level};
    Turret turret{{3.5f,0.25f,15.5f}};
    std::vector<Projectile> projectiles;

    void Run()
    {
        InitWindow(1280,720,"DoomLike FPS");
        DisableCursor();
        SetTargetFPS(60);
        level.Load();

        while(!WindowShouldClose())
        {
            float dt=GetFrameTime();
            player.Update(dt);
            
            turret.Update(dt,player,projectiles,level);
            
            for(auto& p : projectiles)
            {
                if(Vector3Distance(player.pos, p.pos) < player.radius + p.radius){
                    player.TakeDamage(10);
                    p.life = 0;
                }
            }

            if(player.hp<=0){
                player.hp=100;
                player.pos={2.5f,0,2.5f};
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
                level.Draw();
                player.DrawImpacts();
                player.DrawGun();
                turret.Draw();
            
                for(auto& p : projectiles)
                    DrawSphere(p.pos,p.radius,YELLOW);
            EndMode3D();

            DrawFPS(10,10);
            DrawText(TextFormat("HP: %d",(int)player.hp),
                     10,GetScreenHeight()-30,20,RED);
            DrawCrosshair(6,2);
            EndDrawing();
        }
        CloseWindow();
    }
};
