#pragma once
#include "raylib.h"
#include <vector>

struct Enemy;
struct XPOrb;

class DebugMenu
{
public:
    DebugMenu(float width);

    void Draw(float screenWidth, float screenHeight,
              float& playerSpeed,
              float& playerSize,
              float& enemySpeed,
              float& enemySize,
              float& enemySpawnInterval,
              bool& spawnOnClick,
              float& tileSize,
              Texture2D& texGrass,
              Texture2D& texPlayer,
              Texture2D& texEnemy,
              Texture2D& texXP,
              Font& gameFont,
              std::vector<Enemy>& enemies,
              int& playerLevel,
              float& playerXP,
              float& xpToLevel,
              float& xpOrbValue,
              std::vector<XPOrb>& xpOrbs,
              bool* killAllEnemiesFlag);

private:
    float menuWidth;
};
