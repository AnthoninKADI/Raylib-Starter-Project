#include "UpgradeMenu.h"
#include "raylib.h"
#include <cstdlib>   
#include <ctime>     
#include <string>

UpgradeMenu::UpgradeMenu(float screenW, float screenH)
{
    screenWidth = screenW;
    screenHeight = screenH;
    active = false;
}

void UpgradeMenu::Show(const std::vector<UpgradeOption>& options)
{
    currentOptions = options;
    active = true;
}

void UpgradeMenu::Update(PlayerStats& player, std::vector<UpgradeOption>& allUpgrades)
{
    if (!active) return;

    Vector2 mouse = GetMousePosition();
    int cardCount = (int)currentOptions.size();
    float cardW = 300;
    float cardH = 150;
    float spacing = 50;
    float totalWidth = cardCount * cardW + (cardCount - 1) * spacing;
    float startX = screenWidth / 2 - totalWidth / 2;
    float y = screenHeight / 2 - cardH / 2;

    for (int i = 0; i < cardCount; i++)
    {
        Rectangle rect = { startX + i * (cardW + spacing), y, cardW, cardH };
        if (CheckCollisionPointRec(mouse, rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // Appliquer l'upgrade
            currentOptions[i].applyUpgrade(player);

            // Mettre à jour le niveau dans le pool global
            for (auto& upgrade : allUpgrades)
            {
                if (upgrade.name == currentOptions[i].name)
                {
                    upgrade.level++;
                    currentOptions[i].level = upgrade.level;
                    break;
                }
            }

            active = false;
        }
    }
}

void UpgradeMenu::Draw(PlayerStats& player)
{
    if (!active) return;

    float time = GetTime();

    // -----------------------------
    // Fond animé
    // -----------------------------
    for(int i = 0; i < screenHeight; i += 10)
    {
        float t = sin(time + i * 0.05f) * 0.5f + 0.5f;
        DrawRectangle(0, i, screenWidth, 10, { 
            (unsigned char)(20 + 50 * t), 
            (unsigned char)(20 + 50 * t), 
            40, 
            180 
        });
    }

    // -----------------------------
    // Setup cartes
    // -----------------------------
    int cardCount = (int)currentOptions.size();
    float cardW = 320;
    float cardH = 180;
    float spacing = 60;

    float totalWidth = cardCount * cardW + (cardCount - 1) * spacing;
    float startX = screenWidth / 2 - totalWidth / 2;
    float y = screenHeight / 2 - cardH / 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < cardCount; i++)
    {
        Rectangle rect = { startX + i * (cardW + spacing), y, cardW, cardH };
        bool hover = CheckCollisionPointRec(mouse, rect);

        // -----------------------------
        // Animation pop / hover
        // -----------------------------
        float appearAnim = fmin(1.0f, (time * 2.0f - i * 0.2f)); 
        appearAnim = pow(appearAnim, 0.5f);
        float scale = hover ? 1.1f : 0.9f + 0.1f * appearAnim;

        Rectangle drawRect = {
            rect.x - rect.width * (scale - 1) / 2,
            rect.y - rect.height * (scale - 1) / 2,
            rect.width * scale,
            rect.height * scale
        };

        // -----------------------------
        // Ombre / shadow
        // -----------------------------
        DrawRectangleRounded({drawRect.x + 8, drawRect.y + 8, drawRect.width, drawRect.height}, 0.2f, 8, {0,0,0,100});

        // -----------------------------
        // Couleur de la carte selon rareté
        // -----------------------------
        Color baseColor;
        const char* rarityText;
        Color rarityColor;
        switch(currentOptions[i].rarity)
        {
            case 0: baseColor = {150,150,150,255}; rarityText="COMMON"; rarityColor={120,120,120,255}; break;
            case 1: baseColor = {80,150,255,255};  rarityText="RARE";   rarityColor={50,120,255,255}; break;
            case 2: baseColor = {180,0,180,255};   rarityText="EPIC";   rarityColor={150,0,150,255}; break;
            case 3: baseColor = {255,200,0,255};   rarityText="LEGEND"; rarityColor={200,150,0,255}; break;
            default: baseColor = {150,150,150,255}; rarityText="COMMON"; rarityColor={120,120,120,255}; break;
        }

        float glow = (sin(time * 3.0f + i) * 0.5f + 0.5f) * 30;
        Color cardColor = {
            (unsigned char)std::min(255, int(baseColor.r + glow)),
            (unsigned char)std::min(255, int(baseColor.g + glow)),
            (unsigned char)std::min(255, int(baseColor.b + glow)),
            255
        };

        DrawRectangleRounded(drawRect, 0.2f, 8, cardColor);

        if(hover) DrawRectangleRoundedLines(drawRect, 0.2f, 6, WHITE);

        // -----------------------------
        // Texte rareté en haut de la carte avec ombre
        // -----------------------------
        int raritySize = 20;
        int rarityWidth = MeasureText(rarityText, raritySize);
        DrawText(rarityText, drawRect.x  + drawRect.width / 2 - rarityWidth / 2 + 2, drawRect.y + 10 + 2, raritySize, BLACK); 
        DrawText(rarityText, drawRect.x  + drawRect.width / 2 - rarityWidth / 2, drawRect.y + 10, raritySize, rarityColor);

        // Niveau en haut à gauche
        if(currentOptions[i].showLevel) // true sauf heal etc.
        {
            std::string lvlText = "Lvl " + std::to_string(currentOptions[i].level);
            int lvlSize = 18;
            int lvlWidth = MeasureText(lvlText.c_str(), lvlSize);

            // Ombre
            DrawText(lvlText.c_str(), drawRect.x + 5 + 2 + 1, drawRect.y + 5 + 1, lvlSize, BLACK);
            DrawText(lvlText.c_str(), drawRect.x + 5 + 2, drawRect.y + 5, lvlSize, rarityColor);
        }

        // -----------------------------
        // Texte principal (nom & description)
        // -----------------------------
        int titleSize = 28;
        int descSize = 18;

        int titleWidth = MeasureText(currentOptions[i].name.c_str(), titleSize);
        int descWidth  = MeasureText(currentOptions[i].description.c_str(), descSize);

        DrawText(currentOptions[i].name.c_str(), drawRect.x + drawRect.width / 2 - titleWidth / 2 + 2, drawRect.y + 40 + 2, titleSize, BLACK);
        DrawText(currentOptions[i].name.c_str(), drawRect.x + drawRect.width / 2 - titleWidth / 2, drawRect.y + 40, titleSize, WHITE);

        DrawText(currentOptions[i].description.c_str(), drawRect.x + drawRect.width / 2 - descWidth / 2 + 1, drawRect.y + 80 + 1, descSize, BLACK);
        DrawText(currentOptions[i].description.c_str(), drawRect.x + drawRect.width / 2 - descWidth / 2, drawRect.y + 80, descSize, LIGHTGRAY);

        // -----------------------------
        // Texte "changement" (preview)
        // -----------------------------
        if(currentOptions[i].previewText)
        {
            std::string changeText = currentOptions[i].previewText(player);
            int changeSize = 18;
            int changeWidth = MeasureText(changeText.c_str(), changeSize);
            DrawText(changeText.c_str(), drawRect.x + drawRect.width / 2 - changeWidth / 2 + 1, drawRect.y + 120 + 1, changeSize, BLACK);
            DrawText(changeText.c_str(), drawRect.x + drawRect.width / 2 - changeWidth / 2, drawRect.y + 120, changeSize, GREEN);
        }
    }

    // -----------------------------
    // Titre général du menu
    // -----------------------------
    const char* title = "LEVEL UP!";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    float bounce = sin(time * 4.0f) * 5.0f;

    DrawText(title, screenWidth / 2 - titleWidth / 2 + 3, y - 120 + bounce + 3, titleSize, BLACK);
    DrawText(title, screenWidth / 2 - titleWidth / 2, y - 120 + bounce, titleSize, {255,215,0,255});
}

