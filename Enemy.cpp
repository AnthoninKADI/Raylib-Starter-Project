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

void Turret::Draw()const
{
    if(!IsAlive()) return;
    DrawCube(pos,0.3f,1.5f,0.5f,hitFlash>0?ORANGE:BLUE);
}