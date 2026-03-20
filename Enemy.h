#pragma once
#include "raylib.h"
#include <vector>

class Level;

struct Projectile{
    Vector3 pos,dir;
    float life,speed;
    float radius=0.1f;
};

class Turret {
public:
    Vector3 pos;
    float radius=0.35f;
    float fireCooldown=0;
    float fireRate=2.0f;
    float projSpeed=6.0f;
    int hp=30;
    float hitFlash=0;

    Turret(Vector3 p);

    bool IsAlive()const;
    void TakeDamage(int dmg);

    void Update(float dt,Vector3 playerPos,std::vector<Projectile>& projectiles,Level& level);
    void Draw()const;
};