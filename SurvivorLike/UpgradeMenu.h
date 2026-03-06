#pragma once
#include "Entities.h"  // Contient PlayerStats
#include <vector>
#include <string>
#include <functional>

class UpgradeMenu
{
public:
    UpgradeMenu(float screenW, float screenH);

    void Show(const std::vector<UpgradeOption>& options);
    void Update(PlayerStats& player);
    void Draw(PlayerStats& player);

    bool IsActive() const { return active; }

private:
    float screenWidth;
    float screenHeight;
    std::vector<UpgradeOption> currentOptions;
    bool active;
};