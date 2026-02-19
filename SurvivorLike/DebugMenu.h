#pragma once
#include "raylib.h"
#include <vector>
#include <functional>
#include "Entities.h"

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
              bool* killAllEnemiesFlag,
              float& playerHP,
              float& playerMaxHP,
              float& enemySpawnRadius,
              bool& showSpawnRadius,
              std::function<void()> ResetGameFunc,
              std::function<void(int)> LevelUpFunc);

private:
    float menuWidth;
};
