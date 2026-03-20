#include "Player.h"
#include "Enemy.h"
#include "DoomLike.h"
#include "raymath.h"
#include "rlgl.h"
#include <algorithm>
#include <cmath>

Player::Player(Level*l,std::vector<Turret*>& turs):level(l),turrets(turs)
{
    cam.up={0,1,0};
    cam.fovy=60;
    cam.projection=CAMERA_PERSPECTIVE;
}

bool Player::Collides(float x,float z)const
{
    float r=radius+0.01f;
    return level->IsWall(x-r,z-r)||level->IsWall(x+r,z-r)||
           level->IsWall(x-r,z+r)||level->IsWall(x+r,z+r);
}

Vector3 Player::GetGunTip() const
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

void Player::Update(float dt)
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
    if(IsKeyPressed(KEY_R)) Reload();

    for(auto&i:impacts)i.life-=dt;
    impacts.erase(std::remove_if(impacts.begin(),impacts.end(),
        [](auto&i){return i.life<=0;}),impacts.end());

    for(auto&l:lasers)l.life-=dt;
    lasers.erase(std::remove_if(lasers.begin(),lasers.end(),
        [](auto&l){return l.life<=0;}),lasers.end());
}

void Player::Shoot()
{
    if(ammoInClip<=0) return; 
    ammoInClip--;

    Vector3 start = GetGunTip();
    Vector3 dir = Vector3Normalize(Vector3Subtract(cam.target, cam.position));

    const float floorY = 0.0f;
    const float ceilingY = Level::CEILING_Y;
    const float maxDist = 50.0f;
    const float step = 0.05f;

    for(float d = 0; d < maxDist; d += step)
    {
        Vector3 p = Vector3Add(start, Vector3Scale(dir, d));

        for(auto* t : turrets)
        {
            if(t->IsAlive())
            {
                float dx = p.x - t->pos.x;
                float dz = p.z - t->pos.z;
                if(dx*dx + dz*dz < t->radius * t->radius)
                {
                    t->TakeDamage(10);
                    impacts.push_back({p, 0.25f});
                    lasers.push_back({start, p, 0.08f});
                    return;
                }
            }
        }

        if(level->IsWall(p.x, p.z))
        {
            impacts.push_back({p, 0.25f});
            lasers.push_back({start, p, 0.08f});
            return;
        }

        if(p.y <= floorY)
        {
            p.y = floorY;
            impacts.push_back({p, 0.25f});
            lasers.push_back({start, p, 0.08f});
            return;
        }

        if(p.y >= ceilingY)
        {
            p.y = ceilingY;
            impacts.push_back({p, 0.25f});
            lasers.push_back({start, p, 0.08f});
            return;
        }
    }
}

void Player::Reload()
{
    int needed = clipSize - ammoInClip;
    if(needed>0 && ammoStock>0)
    {
        int taken = (ammoStock >= needed) ? needed : ammoStock;
        ammoInClip += taken;
        ammoStock -= taken;
    }
}

void Player::DrawGun()
{
    rlPushMatrix();
    rlTranslatef(cam.position.x, cam.position.y, cam.position.z);
    rlRotatef(RAD2DEG*yaw,0,1,0);
    rlRotatef(RAD2DEG*-pitch,1,0,0);
    rlTranslatef(0,-0.25f,0.6f);
    DrawCube({0,0,0},0.15f,0.2f,0.4f,DARKGRAY);
    rlPopMatrix();
}

void Player::DrawLasers()
{
    for(auto&l:lasers) 
        DrawCylinderEx(l.start,l.end,0.03f,0.03f,6,Color{0,255,255,200});
}

void Player::DrawImpacts()
{
    for(auto&i:impacts) 
        DrawSphere(i.pos,0.08f,RED);
}

void Player::DrawHpBar()
{
    int w = 200; int h = 20; int x = 20; int y = GetScreenHeight()-40;
    DrawRectangle(x-2,y-2,w+4,h+4,BLACK);
    DrawRectangle(x,y,w*(hp/(float)maxHp),h,RED);
}

void Player::DrawAmmoUI()
{
    int x = 20; int y = GetScreenHeight()-70;
    DrawText(TextFormat("Ammo: %d / %d", ammoInClip, ammoStock), x, y, 20, WHITE);
}