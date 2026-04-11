#include "Enemy.h"
#include "DoomLike.h"
#include "raymath.h"
#include <algorithm>

Turret::Turret(Vector3 p):pos(p){}

bool Turret::IsAlive()const{return hp>0;}

void Turret::TakeDamage(int dmg)
{
    hp-=dmg;
    hitFlash=0.1f;
}

void Turret::Update(float dt,Vector3 playerPos,std::vector<Projectile>& projectiles,Level& level)
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

void Turret::Draw(Vector3 playerPos) const
{
    if (!IsAlive()) return;

    // === Direction vers le joueur ===
    Vector3 dir = Vector3Subtract(playerPos, pos);
    dir.y = 0;
    dir = Vector3Normalize(dir);

    // === Couleurs ===
    Color baseColor = DARKGRAY;
    Color bodyColor = (hitFlash > 0) ? ORANGE : GRAY;
    Color gunColor  = BLACK;
    Color eyeColor  = (fireCooldown <= 0) ? RED : MAROON;

    // === BASE (socle) ===
    DrawCylinder(
        {pos.x, pos.y - 0.25f, pos.z},
        0.5f, 0.5f,
        0.2f,
        16,
        baseColor
    );

    // === CORPS ===
    DrawCube(
        {pos.x, pos.y + 0.3f, pos.z},
        0.6f, 0.6f, 0.6f,
        bodyColor
    );

    // === TÊTE ===
    Vector3 headPos = {pos.x, pos.y + 0.7f, pos.z};
    DrawCube(
        headPos,
        0.4f, 0.4f, 0.4f,
        bodyColor
    );

    // === CANON orienté vers le joueur ===
    Vector3 barrelEnd = {
        headPos.x + dir.x * 0.8f,
        headPos.y,
        headPos.z + dir.z * 0.8f
    };

    DrawCylinderEx(
        headPos,
        barrelEnd,
        0.1f,
        0.1f,
        8,
        gunColor
    );

    // === "ŒIL" lumineux ===
    DrawSphere(
        {
            headPos.x + dir.x * 0.25f,
            headPos.y,
            headPos.z + dir.z * 0.25f
        },
        0.08f,
        eyeColor
    );
}