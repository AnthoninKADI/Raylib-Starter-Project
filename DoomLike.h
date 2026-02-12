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

struct Impact{Vector3 pos;float life;};
struct Laser{Vector3 start,end;float life;};
struct Projectile{Vector3 pos,dir;float life,speed,radius=0.1f;};

class Turret {
public:
    Vector3 pos;
    float radius=0.35f;
    float fireCooldown=0;
    float fireRate=2.0f;
    float projSpeed=6.0f;
    int hp=30;
    float hitFlash=0;

    Turret(Vector3 p):pos(p){}

    bool IsAlive()const{return hp>0;}
    void TakeDamage(int dmg){hp-=dmg; hitFlash=0.1f;}

    void Update(float dt, Vector3 playerPos, std::vector<Projectile>& projectiles, Level& level)
    {
        if(hitFlash>0) hitFlash-=dt;
        if(fireCooldown>0) fireCooldown-=dt;

        if(IsAlive() && fireCooldown<=0)
        {
            Vector3 dir=Vector3Normalize(Vector3Subtract(playerPos,pos));
            dir.y=0;
            projectiles.push_back({pos,dir,3.0f,projSpeed});
            fireCooldown=fireRate;
        }

        for(auto& p:projectiles)
        {
            p.pos=Vector3Add(p.pos,Vector3Scale(p.dir,p.speed*dt));
            p.life-=dt;
            if(level.IsWall(p.pos.x,p.pos.z)) p.life=0;
        }

        projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),
            [](auto&p){return p.life<=0;}),projectiles.end());
    }

    void Draw()const
    {
        if(!IsAlive()) return;
        DrawCube(pos,0.3f,1.5f,0.5f,hitFlash>0?ORANGE:BLUE);
    }
};

class Player {
public:
    Camera3D cam;
    Vector3 pos={2.5f,0,2.5f};
    float yaw=0,pitch=0;

    float baseSpeed=3.0f;
    float sprintMult=1.6f;
    float crouchMult=0.5f;
    float currentSpeed=3.0f;
    float speedSmooth=10.0f;

    float radius=0.3f;
    float velY=0;
    bool onGround=false;

    const float gravity=14.0f;
    const float jumpForce=4.0f;

    const float standHeight=1.0f;
    const float crouchHeight=0.6f;
    float camHeight=1.0f;
    float heightSmooth=12.0f;

    float bobPhase=0;
    float bobSpeed=8.0f;
    float bobX=0.03f;
    float bobY=0.05f;

    Level* level;
    std::vector<Turret*> turrets;

    std::vector<Impact> impacts;
    std::vector<Laser> lasers;

    int hp=100;
    int maxHp=100;

    Player(Level*l, std::vector<Turret*> ts):level(l), turrets(ts)
    {
        cam.up={0,1,0};
        cam.fovy=60;
        cam.projection=CAMERA_PERSPECTIVE;
    }

    bool Collides(float x,float z)const
    {
        float r=radius+0.01f;
        return level->IsWall(x-r,z-r)||level->IsWall(x+r,z-r)||
               level->IsWall(x-r,z+r)||level->IsWall(x+r,z+r);
    }

    Vector3 GetGunTip() const
    {
        Vector3 forward = { sinf(yaw)*cosf(pitch), sinf(pitch), cosf(yaw)*cosf(pitch) };
        Vector3 right   = { -forward.z, 0, forward.x };
        Vector3 up      = {0,1,0};

        Vector3 gunOffset = { 0.0f, -0.25f, 0.6f };
        Vector3 basePos = Vector3Add(pos, {0, camHeight, 0});

        return Vector3Add(
            Vector3Add(basePos, Vector3Scale(right, gunOffset.x)),
            Vector3Add(Vector3Scale(up, gunOffset.y), Vector3Scale(forward, gunOffset.z))
        );
    }

    void Update(float dt)
    {
        Vector2 m=GetMouseDelta();
        yaw-=m.x*0.003f;
        pitch=Clamp(pitch-m.y*0.003f,-1.2f,1.2f);

        Vector3 f={sinf(yaw),0,cosf(yaw)};
        Vector3 r={-f.z,0,f.x};
        Vector3 mv={0};

        if(IsKeyDown(KEY_W)) mv=Vector3Add(mv,f);
        if(IsKeyDown(KEY_S)) mv=Vector3Subtract(mv,f);
        if(IsKeyDown(KEY_A)) mv=Vector3Subtract(mv,r);
        if(IsKeyDown(KEY_D)) mv=Vector3Add(mv,r);

        float moveLen=Vector3Length(mv);
        if(moveLen>0) mv=Vector3Normalize(mv);

        float targetSpeed=baseSpeed;
        if(IsKeyDown(KEY_LEFT_SHIFT)) targetSpeed*=sprintMult;
        if(IsKeyDown(KEY_LEFT_CONTROL)) targetSpeed*=crouchMult;
        currentSpeed=Lerp(currentSpeed,targetSpeed,speedSmooth*dt);

        Vector3 step=Vector3Scale(mv,currentSpeed*dt);
        if(!Collides(pos.x+step.x,pos.z)) pos.x+=step.x;
        if(!Collides(pos.x,pos.z+step.z)) pos.z+=step.z;

        if(onGround && IsKeyPressed(KEY_SPACE)){velY=jumpForce; onGround=false;}
        velY-=gravity*dt;
        pos.y+=velY*dt;
        if(pos.y<=0){pos.y=0; velY=0; onGround=true;}

        float targetHeight=IsKeyDown(KEY_LEFT_CONTROL)?crouchHeight:standHeight;
        camHeight=Lerp(camHeight,targetHeight,heightSmooth*dt);

        float ceilingLimit=Level::CEILING_Y-camHeight;
        if(pos.y>ceilingLimit){pos.y=ceilingLimit; velY=0;}

        float bobAmount=(moveLen>0 && onGround)?currentSpeed/baseSpeed:0;
        if(bobAmount>0) bobPhase+=dt*bobSpeed*bobAmount;
        else bobPhase=0;

        float bx=sinf(bobPhase)*bobX*bobAmount;
        float by=fabsf(cosf(bobPhase))*bobY*bobAmount;

        cam.position={pos.x+bx, pos.y+camHeight+by, pos.z};
        cam.target=Vector3Add(cam.position, {sinf(yaw)*cosf(pitch), sinf(pitch), cosf(yaw)*cosf(pitch)});

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) Shoot();

        for(auto&i:impacts)i.life-=dt;
        impacts.erase(std::remove_if(impacts.begin(),impacts.end(),
            [](auto&i){return i.life<=0;}),impacts.end());

        for(auto&l:lasers)l.life-=dt;
        lasers.erase(std::remove_if(lasers.begin(),lasers.end(),
            [](auto&l){return l.life<=0;}),lasers.end());
    }

    void Shoot()
    {
        Vector3 start = GetGunTip();
        Vector3 dir = Vector3Normalize(Vector3Subtract(cam.target, cam.position));

        const float floorY = 0.0f;
        const float ceilingY = Level::CEILING_Y;
        const float maxDist = 50.0f;
        const float step = 0.05f;

        for(float d = 0; d < maxDist; d += step)
        {
            Vector3 p = Vector3Add(start, Vector3Scale(dir, d));

            for(auto*turret: turrets)
            {
                if(!turret->IsAlive()) continue;
                float dx = p.x - turret->pos.x;
                float dz = p.z - turret->pos.z;
                if(dx*dx + dz*dz < turret->radius*turret->radius)
                {
                    turret->TakeDamage(10);
                    impacts.push_back({p, 0.25f});
                    lasers.push_back({start, p, 0.08f});
                    return;
                }
            }

            if(level->IsWall(p.x, p.z))
            {
                impacts.push_back({p, 0.25f});
                lasers.push_back({start, p, 0.08f});
                return;
            }

            if(p.y <= floorY){p.y=floorY; impacts.push_back({p,0.25f}); lasers.push_back({start,p,0.08f}); return;}
            if(p.y >= ceilingY){p.y=ceilingY; impacts.push_back({p,0.25f}); lasers.push_back({start,p,0.08f}); return;}
        }
    }

    void DrawGun()
    {
        rlPushMatrix();
        rlTranslatef(cam.position.x, cam.position.y, cam.position.z);
        rlRotatef(RAD2DEG*yaw,0,1,0);
        rlRotatef(RAD2DEG*-pitch,1,0,0);
        rlTranslatef(0,-0.25f,0.6f);
        DrawCube({0,0,0},0.15f,0.2f,0.4f,DARKGRAY);
        rlPopMatrix();
    }

    void DrawLasers(){for(auto&l:lasers)
        DrawCylinderEx(l.start,l.end,0.03f,0.03f,6,Color{0,255,255,200});}

    void DrawImpacts(){for(auto&i:impacts)DrawSphere(i.pos,0.08f,RED);}

    void DrawHpBar()
    {
        int barW = 200;
        int barH = 20;
        int x = 20;
        int y = GetScreenHeight()-40;
        DrawRectangle(x-2,y-2,barW+4,barH+4,BLACK);
        float ratio = (float)hp / (float)maxHp;
        DrawRectangle(x,y,barW*ratio,barH,RED);
    }
};

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

    Game()
    {
        for(auto& t : turrets) turretPtrs.push_back(&t);
        player.turrets = turretPtrs;
    }

    void Reset()
    {
        player.pos = {2.5f,0,2.5f};
        player.velY = 0;
        player.hp = player.maxHp;
        player.camHeight = player.standHeight;
        player.impacts.clear();
        player.lasers.clear();
        player.yaw = 0;
        player.pitch = 0;

        for(auto& t : turrets) { t.hp = 30; t.fireCooldown = 0; }

        projectiles.clear();
    }

    void DrawMinimap()
    {
        const int mapSize = 200;
        const int margin = 20;
        const float scale = mapSize / (Level::W * Level::CELL);

        int startX = GetScreenWidth() - mapSize - margin;
        int startY = margin;

        DrawRectangle(startX - 4, startY - 4, mapSize + 8, mapSize + 8, BLACK);
        DrawRectangle(startX, startY, mapSize, mapSize, DARKGRAY);

        for(int z = 0; z < Level::H; z++)
            for(int x = 0; x < Level::W; x++)
                if(level.grid[z][x] == 1)
                {
                    int rx = startX + x * Level::CELL * scale;
                    int ry = startY + z * Level::CELL * scale;
                    int cellSize = Level::CELL * scale;
                    DrawRectangle(rx, ry, cellSize, cellSize, GRAY);
                }

        float px = startX + player.pos.x * scale;
        float pz = startY + player.pos.z * scale;
        float angle = player.yaw;
        float size = 10.0f;

        Vector2 tip = { px + sinf(angle) * size, pz + cosf(angle) * size };
        Vector2 left = { px + sinf(angle + 2.5f) * size * 0.6f, pz + cosf(angle + 2.5f) * size * 0.6f };
        Vector2 right = { px + sinf(angle - 2.5f) * size * 0.6f, pz + cosf(angle - 2.5f) * size * 0.6f };
        DrawTriangle(tip, left, right, BLUE);

        for(auto& t : turrets)
            if(t.IsAlive())
            {
                int tx = startX + t.pos.x * scale;
                int tz = startY + t.pos.z * scale;
                DrawCircle(tx, tz, 6, RED);
            }
    }

    void Run()
    {
        InitWindow(1280,720,"DoomLike");
        DisableCursor();
        SetTargetFPS(60);
        level.Load();

        while(!WindowShouldClose())
        {
            float dt = GetFrameTime();
            player.Update(dt);

            for(auto& t: turrets)
                t.Update(dt, player.pos, projectiles, level);

            for(auto&p : projectiles)
                if(Vector3Distance(player.pos,p.pos) < player.radius + p.radius)
                    { player.hp -= 10; p.life = 0; }

            if(player.hp <= 0) Reset();

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(player.cam);
                level.Draw();
                for(auto& t: turrets) t.Draw();
                player.DrawLasers();
                player.DrawImpacts();
                player.DrawGun();
                for(auto&p:projectiles) DrawSphere(p.pos,p.radius,YELLOW);
            EndMode3D();

            DrawFPS(10,10);
            player.DrawHpBar();
            DrawCrosshair(6,2);
            DrawMinimap();

            EndDrawing();
        }
        CloseWindow();
    }
};
