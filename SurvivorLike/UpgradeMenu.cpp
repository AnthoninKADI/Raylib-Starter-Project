#include "UpgradeMenu.h"
#include "raylib.h"

UpgradeMenu::UpgradeMenu(float sw, float sh)
{
    screenWidth = sw;
    screenHeight = sh;
    active = false;
}

void UpgradeMenu::Show(const std::vector<UpgradeOption>& options)
{
    currentOptions = options;
    active = true;
}

void UpgradeMenu::Update(PlayerStats& player)
{
    if(!active) return;

    float cardWidth = 300;
    float cardHeight = 200;
    float spacing = 50;
    float startX = screenWidth/2 - (cardWidth*3 + spacing*2)/2;
    float startY = screenHeight/2 - cardHeight/2;

    for(int i=0;i<3;i++)
    {
        Rectangle card = { startX + i*(cardWidth+spacing), startY, cardWidth, cardHeight };
        if(CheckCollisionPointRec(GetMousePosition(), card) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if(i < currentOptions.size())
            {
                currentOptions[i].applyUpgrade(player);
                active = false;
            }
        }
    }
}

void UpgradeMenu::Draw()
{
    if(!active) return;

    DrawRectangle(0,0,screenWidth,screenHeight,Color{0,0,0,150});

    float cardWidth = 300;
    float cardHeight = 200;
    float spacing = 50;
    float startX = screenWidth/2 - (cardWidth*3 + spacing*2)/2;
    float startY = screenHeight/2 - cardHeight/2;

    for(int i=0;i<3;i++)
    {
        Rectangle card = { startX + i*(cardWidth+spacing), startY, cardWidth, cardHeight };
        DrawRectangleRounded(card, 0.2f, 10, LIGHTGRAY);
        DrawRectangleRoundedLines(card, 0.2f, 10, WHITE);
        if(i < currentOptions.size())
        {
            DrawText(currentOptions[i].name.c_str(), card.x+20, card.y+20, 24, WHITE);
            DrawText(currentOptions[i].description.c_str(), card.x+20, card.y+60, 18, WHITE);
        }
    }
}