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

void UpgradeMenu::Update(PlayerStats& player)
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
            currentOptions[i].applyUpgrade(player);
            active = false;
        }
    }
}

void UpgradeMenu::Draw()
{
    if (!active) return;

    float time = GetTime();

    // ===== Fond animé =====
    for(int i = 0; i < screenHeight; i += 10)
    {
        float t = sin(time + i * 0.05f) * 0.5f + 0.5f;
        DrawRectangle(0, i, screenWidth, 10, { (unsigned char)(20 + 50*t), (unsigned char)(20 + 50*t), 40, 180 });
    }

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

        // ===== Effet de "pop" au survol ou apparition =====
        float appearAnim = fmin(1.0f, (time * 2.0f - i * 0.2f));
        appearAnim = pow(appearAnim, 0.5f);
        float scale = hover ? 1.15f : 0.9f + 0.1f * appearAnim;

        Rectangle drawRect = {
            rect.x - rect.width * (scale - 1) / 2,
            rect.y - rect.height * (scale - 1) / 2,
            rect.width * scale,
            rect.height * scale
        };

        // Shadow de la carte
        DrawRectangleRounded({drawRect.x + 8, drawRect.y + 8, drawRect.width, drawRect.height}, 0.2f, 8, {0,0,0,100});

        // Glow pour rareté
        Color baseColor;
        std::string rarityText;
        Color rarityColor;

        switch(currentOptions[i].rarity)
        {
            case 0: baseColor = {150,150,150,255}; rarityText="COMMON"; rarityColor={120,120,120,255}; break;
            case 1: baseColor = {80,150,255,255}; rarityText="RARE"; rarityColor={50,100,255,255}; break;
            case 2: baseColor = {180,0,180,255}; rarityText="EPIC"; rarityColor={140,0,140,255}; break;
            case 3: baseColor = {255,200,0,255}; rarityText="LEGENDARY"; rarityColor={200,150,0,255}; break;
        }

        float glow = (sin(time * 3.0f + i) * 0.5f + 0.5f) * 40;
        Color cardColor = {
            (unsigned char)std::min(255, int(baseColor.r + glow)),
            (unsigned char)std::min(255, int(baseColor.g + glow)),
            (unsigned char)std::min(255, int(baseColor.b + glow)),
            255
        };
        DrawRectangleRounded(drawRect, 0.2f, 8, cardColor);

        // Contour de la carte
        DrawRectangleRoundedLines(drawRect, 0.2f, 8, WHITE);

        // ===== Texte de rareté au-dessus du titre avec ombre =====
        int raritySize = 22;
        int rarityWidth = MeasureText(rarityText.c_str(), raritySize);

        // Ombre
        DrawText(rarityText.c_str(),
            drawRect.x + drawRect.width/2 - rarityWidth/2 + 2,
            drawRect.y + 10 + 2,
            raritySize,
            {0,0,0,150});
        // Texte principal
        DrawText(rarityText.c_str(),
            drawRect.x + drawRect.width/2 - rarityWidth/2,
            drawRect.y + 10,
            raritySize,
            rarityColor);

        // ===== Texte principal de la carte =====
        int titleSize = 30;
        int descSize = 18;
        std::string name = currentOptions[i].name;
        std::string desc = currentOptions[i].description;

        int titleWidth = MeasureText(name.c_str(), titleSize);
        int descWidth  = MeasureText(desc.c_str(), descSize);

        // Ombre du titre
        DrawText(name.c_str(),
            drawRect.x + drawRect.width/2 - titleWidth/2 + 2,
            drawRect.y + 40 + 2,
            titleSize,
            {0,0,0,150});
        DrawText(name.c_str(),
            drawRect.x + drawRect.width/2 - titleWidth/2,
            drawRect.y + 40,
            titleSize,
            WHITE);

        // Ombre de la description
        DrawText(desc.c_str(),
            drawRect.x + drawRect.width/2 - descWidth/2 + 1,
            drawRect.y + 90 + 1,
            descSize,
            {0,0,0,120});
        DrawText(desc.c_str(),
            drawRect.x + drawRect.width/2 - descWidth/2,
            drawRect.y + 90,
            descSize,
            LIGHTGRAY);
    }

    // ===== Titre "LEVEL UP" =====
    const char* title = "LEVEL UP!";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    float bounce = sin(time * 4.0f) * 5.0f;

    // Ombre du titre
    DrawText(title,
        screenWidth/2 - titleWidth/2 + 2,
        y - 120 + bounce + 2,
        titleSize,
        {0,0,0,180});
    // Texte principal
    DrawText(title,
        screenWidth/2 - titleWidth/2,
        y - 120 + bounce,
        titleSize,
        {255,215,0,255});
}