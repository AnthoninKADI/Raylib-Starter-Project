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

    void Draw3D() const
    {
        for(int z=0; z<H; z++)
            for(int x=0; x<W; x++)
                if(grid[z][x]==1) {
                    Vector3 p = {x*CELL+CELL/2,1,z*CELL+CELL/2};
                    DrawCube(p,CELL,2,CELL,DARKGRAY);
                    DrawCubeWires(p,CELL,2,CELL,BLACK);
                }
    }
};

struct Impact {
    Vector3 pos;
    float life;
};

class Player {
public:
    Camera3D cam;
    Vector3 pos = {2.5f,0,2.5f};

    float yaw=0, pitch=0;
    float speed=3.0f, radius=0.3f;

    float bobPhase=0;
    float bobSpeed=10.0f;
    float bobY=0.05f;
    float bobX=0.03f;
    
    float recoil=0;

    Level* level;
    std::vector<Impact> impacts;

    Player(Level* lvl):level(lvl)
    {
        cam.up={0,1,0};
        cam.fovy=60;
        cam.projection=CAMERA_PERSPECTIVE;
    }

    bool IsWall(float x,float z) const
    {
        float offset = radius + 0.01f;
        int minX = (int)floor((x - offset)/Level::CELL);
        int maxX = (int)floor((x + offset)/Level::CELL);
        int minZ = (int)floor((z - offset)/Level::CELL);
        int maxZ = (int)floor((z + offset)/Level::CELL);

        for(int gz=minZ; gz<=maxZ; gz++)
            for(int gx=minX; gx<=maxX; gx++)
            {
                if(gx<0||gx>=Level::W||gz<0||gz>=Level::H) return true;
                if(level->grid[gz][gx]==1) return true;
            }
        return false;
    }
    
    void Update(float dt)
    {
        Vector2 m=GetMouseDelta();
        yaw-=m.x*0.003f;
        pitch-=m.y*0.003f;
        pitch=Clamp(pitch,-1.2f,1.2f);

        pitch+=recoil;
        recoil=Lerp(recoil,0,dt*15);
        
        Vector3 f={sinf(yaw),0,cosf(yaw)};
        Vector3 r={-f.z,0,f.x};
        Vector3 mv={0};
        
        if(IsKeyDown(KEY_W)) mv=Vector3Add(mv,f);
        if(IsKeyDown(KEY_S)) mv=Vector3Subtract(mv,f);
        if(IsKeyDown(KEY_A)) mv=Vector3Subtract(mv,r);
        if(IsKeyDown(KEY_D)) mv=Vector3Add(mv,r);

        bool moving=Vector3Length(mv)>0.01f;
        if(moving) mv=Vector3Normalize(mv);

        Vector3 vel=Vector3Scale(mv,speed*dt);
        Vector3 np=pos;

        if(!IsWall(pos.x+vel.x,pos.z)) np.x+=vel.x;
        if(!IsWall(np.x,pos.z+vel.z)) np.z+=vel.z;
        pos=np;
        
        if(moving) bobPhase+=dt*bobSpeed;
        else bobPhase=0;

        float bx=sinf(bobPhase*0.5f)*bobX;
        float by=sinf(bobPhase)*bobY;
        
        cam.position={pos.x+bx,1.0f+by,pos.z};
        Vector3 dir={
            sinf(yaw)*cosf(pitch),
            sinf(pitch),
            cosf(yaw)*cosf(pitch)
        };
        cam.target=Vector3Add(cam.position,dir);
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            Shoot();
        
        for(auto&i:impacts) i.life-=dt;
        impacts.erase(std::remove_if(impacts.begin(),impacts.end(),
            [](auto&i){return i.life<=0;}),impacts.end());
    }
    
    void Shoot()
    {
        Vector3 dir=Vector3Normalize(Vector3Subtract(cam.target,cam.position));
        recoil=0.001f;

        float maxDist = 50.0f;
        float step = 0.05f;

        for(float d=0; d<maxDist; d+=step)
        {
            Vector3 p=Vector3Add(cam.position,Vector3Scale(dir,d));
            int gx=(int)floor(p.x/Level::CELL);
            int gz=(int)floor(p.z/Level::CELL);

            if(gx<0||gz<0||gx>=Level::W||gz>=Level::H) break;

            if(level->grid[gz][gx]==1){
                impacts.push_back({p,0.2f});
                break;
            }
        }
    }

    void DrawGun() const
    {
        Color HAND={220,190,160,255};
        Color GUN ={100,100,100,255};
        Color OUTLINE={0,0,0,255};

        rlPushMatrix();

        rlTranslatef(cam.position.x, cam.position.y, cam.position.z);
        rlRotatef(RAD2DEG*yaw, 0,1,0);
        rlRotatef(RAD2DEG*-pitch, 1,0,0);
        rlTranslatef(-0.45f, -0.25f, 0.7f);


        DrawCube({0,-0.05f,0},0.15f,0.2f,0.2f,HAND);


        DrawCube({0,-0.15f,0.15f},0.1f,0.35f,0.15f,GUN);
        DrawCubeWires({0,-0.15f,0.15f},0.1f,0.35f,0.15f,OUTLINE);

        DrawCube({0,0.05f,0.5f},0.18f,0.12f,0.6f,GUN);
        DrawCubeWires({0,0.05f,0.5f},0.18f,0.12f,0.6f,OUTLINE);

        DrawCube({0,0.05f,0.85f},0.08f,0.1f,0.1f,GUN);
        DrawCubeWires({0,0.05f,0.85f},0.08f,0.1f,0.1f,OUTLINE);

        rlPopMatrix();
    }
    
    void DrawImpacts() const
    {
        for(auto&i:impacts)
            DrawSphere(i.pos,0.08f,RED);
    }
};

class Game {
public:
    Level level;
    Player player{&level};

    void Run()
    {
        InitWindow(800,600,"DoomLike FPS");
        DisableCursor();
        SetTargetFPS(60);

        while(!WindowShouldClose())
        {
            float dt=GetFrameTime();
            player.Update(dt);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
            level.Draw3D();
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
