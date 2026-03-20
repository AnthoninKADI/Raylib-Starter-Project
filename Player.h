#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

class Level;
class Turret;

struct Impact{Vector3 pos;float life;};
struct Laser{Vector3 start,end;float life;};

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
    
    int ammoInClip = 10;
    int clipSize = 10;
    int ammoStock = 30;
    
    float damageFlash = 0.0f;

    Player(Level*l,std::vector<Turret*>& turs);

    bool Collides(float x,float z)const;
    Vector3 GetGunTip() const;

    void Update(float dt);
    void Shoot();
    void Reload();

    void DrawGun();
    void DrawLasers();
    void DrawImpacts();

    void DrawHpBar();
    void DrawAmmoUI();
};