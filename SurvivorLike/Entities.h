#pragma once
#include <functional>
#include <string>

#include "raylib.h"
#include <vector>

struct Enemy
{
    Vector2 pos;
    float speed;
    float size;
    Texture2D texture;
};

struct XPOrb
{
    Vector2 pos;
    float size;
    int value;
    Texture2D texture;
};

struct Projectile
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    bool active;
    float range;
    float travelled;
    int pierceCount;
    int ricochetCount;
};

struct PlayerStats
{
    float moveSpeed;
    float lifeSteal;
    float damage;
    float projectileCooldown;
    float maxHP;
    int projectileCount;
    float effectDuration;
    int projectilePierce; 
};

struct UpgradeOption
{
    std::string name;
    std::string description;
    std::function<void(PlayerStats&)> applyUpgrade;
    int rarity;
    std::function<std::string(PlayerStats&)> previewText;
    int level = 1;
};