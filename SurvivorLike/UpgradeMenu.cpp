#include "UpgradeMenu.h"
#include "raylib.h"
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

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 200});

    int cardCount = (int)currentOptions.size();
    float cardW = 300;
    float cardH = 160;
    float spacing = 60;

    float totalWidth = cardCount * cardW + (cardCount - 1) * spacing;
    float startX = screenWidth / 2 - totalWidth / 2;
    float y = screenHeight / 2 - cardH / 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < cardCount; i++)
    {
        Rectangle rect = { startX + i * (cardW + spacing), y, cardW, cardH };

        bool hover = CheckCollisionPointRec(mouse, rect);

        if (hover)
        {
            rect.x -= 5;
            rect.y -= 5;
            rect.width += 10;
            rect.height += 10;
        }

        Rectangle shadow = { rect.x + 8, rect.y + 8, rect.width, rect.height };
        DrawRectangleRounded(shadow, 0.15f, 8, Color{0,0,0,120});

        Color bg = hover ? Color{200,120,255,255} : Color{120,40,180,255};
        DrawRectangleRounded(rect, 0.15f, 8, bg);

        DrawRectangleRoundedLines(rect, 0.15f, 8, WHITE);

        int titleSize = 28;
        int descSize = 18;

        int titleWidth = MeasureText(currentOptions[i].name.c_str(), titleSize);
        int descWidth = MeasureText(currentOptions[i].description.c_str(), descSize);

        DrawText(
            currentOptions[i].name.c_str(),
            rect.x + rect.width/2 - titleWidth/2,
            rect.y + 30,
            titleSize,
            WHITE
        );

        DrawText(
            currentOptions[i].description.c_str(),
            rect.x + rect.width/2 - descWidth/2,
            rect.y + 80,
            descSize,
            LIGHTGRAY
        );
    }

    const char* title = "LEVEL UP";
    int titleSize = 42;
    int titleWidth = MeasureText(title, titleSize);

    DrawText(
        title,
        screenWidth/2 - titleWidth/2,
        y - 90,
        titleSize,
        GOLD
    );
}