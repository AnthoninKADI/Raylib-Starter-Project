#include "MainMenu.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>  // pour rand()
#include <ctime>    // pour srand()

MainMenu::MainMenu(float sw, float sh)
{
    screenWidth = sw;
    screenHeight = sh;

    state = MenuState::Main;
    startGame = false;
    quitGame = false;

    float buttonWidth = 300;
    float buttonHeight = 60;
    float centerX = screenWidth / 2.0f - buttonWidth / 2.0f;
    float startY = screenHeight / 2.0f - 150;

    playButton = { centerX, startY, buttonWidth, buttonHeight };
    charactersButton = { centerX, startY + 80, buttonWidth, buttonHeight };
    optionsButton = { centerX, startY + 160, buttonWidth, buttonHeight };
    quitButton = { centerX, startY + 240, buttonWidth, buttonHeight };
    backButton = { 30, 30, 40, 40 };

    characters = {
        { "Slime", "Balanced fighter", GREEN },
        { "Warrior", "High HP and damage", RED },
        { "Rogue", "Fast and agile", BLUE },
        { "Mage", "Strong ranged attacks", PURPLE },
        { "Tank", "Massive defense", ORANGE },
        { "Hunter", "Precision striker", YELLOW },
        { "Necro", "Summoner type", DARKPURPLE },
        { "Knight", "Solid frontline", SKYBLUE }
    };

    selectedCharacter = 0;
    scrollOffset = 0.0f;
    contentHeight = 0.0f;
    fullscreen = false;
    mouseSensitivity = 1.0f;
    aimMode = AimMode::ClosestEnemy;
    titlePulse = 0.0f;
    backgroundTime = 0.0f;
    fadeAlpha = 0.0f;

    // Initialiser particules
    srand((unsigned int)time(0));
    for (int i = 0; i < 80; i++) {
        Particle p;
        p.x = (float)(rand() % (int)screenWidth);
        p.y = (float)(rand() % (int)screenHeight);
        p.size = 1.0f + (rand() % 3);
        p.speed = 10.0f + (rand() % 20);
        particles.push_back(p);
    }
}

void MainMenu::Update()
{
    float dt = GetFrameTime();
    fadeAlpha += dt;
    if (fadeAlpha > 1.0f) fadeAlpha = 1.0f;

    backgroundTime += dt;
    titlePulse += GetFrameTime() * 5.0f; 

    // Mise à jour particules
    for (auto &p : particles) {
        p.y += p.speed * dt;
        if (p.y > screenHeight) {
            p.y = 0;
            p.x = (float)(rand() % (int)screenWidth);
        }
    }

    // Menu logic
    if (state == MenuState::Main) {
        if (ButtonLogic(playButton)) startGame = true;
        if (ButtonLogic(charactersButton)) state = MenuState::Characters;
        if (ButtonLogic(optionsButton)) state = MenuState::Options;
        if (ButtonLogic(quitButton)) quitGame = true;
    } 
    else if (state == MenuState::Characters) {
        if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            state = MenuState::Main;

        float wheel = GetMouseWheelMove();
        scrollOffset -= wheel * 40.0f;

        int columns = 4;
        float spacing = 30;
        float cardHeight = 220;
        int rows = (characters.size() + columns - 1) / columns;
        contentHeight = rows * (cardHeight + spacing);

        if(scrollOffset < 0) scrollOffset = 0;
        if(scrollOffset > contentHeight - screenHeight + 120.0f)
            scrollOffset = contentHeight - screenHeight + 120.0f;
    } 
    else {
        if (CheckCollisionPointRec(GetMousePosition(), backButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            state = MenuState::Main;
    }
}

void MainMenu::Draw()
{
    // ===== BACKGROUND NUIT + PARTICULES =====
    for (int y = 0; y < screenHeight; y++) {
        float t = (float)y / screenHeight;
        Color col = {
            (unsigned char)(15 + 15*t),
            (unsigned char)(15 + 15*t),
            (unsigned char)(40 + 40*t),
            255
        };
        DrawLine(0, y, screenWidth, y, col);
    }

    for (auto &p : particles) {
        DrawCircle(p.x, p.y, p.size, Color{200,200,255,150});
    }

    // ===== MAIN MENU =====
    if (state == MenuState::Main) {
        const char* title = "VAMPIRE SURVIVOR";
        float pulse = 1.0f + 0.05f * sin(titlePulse); // pulse 5% autour de 1.0
        int fontSize = (int)(80 * pulse);
        int textWidth = MeasureText(title, fontSize);
        DrawText(title, screenWidth/2 - textWidth/2, 140, fontSize, WHITE);
        int centerX = screenWidth / 2 - textWidth / 2;

        DrawText(title, centerX, 140, fontSize, WHITE);

        auto DrawFancyButton = [&](Rectangle rect, const char* text) {
            bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
            float scale = hover ? 1.08f : 1.0f;

            Rectangle scaled = {
                rect.x - (rect.width*(scale-1)/2),
                rect.y - (rect.height*(scale-1)/2),
                rect.width*scale,
                rect.height*scale
            };

            DrawRectangleRounded(scaled, 0.3f, 8, hover ? Color{60,60,90,255} : Color{40,40,60,255});
            if (hover)
                DrawRectangleRoundedLines(scaled, 0.3f, 8, WHITE);

            int fontSize = 30;
            int textWidth = MeasureText(text, fontSize);
            DrawText(text, scaled.x + scaled.width/2 - textWidth/2,
                     scaled.y + scaled.height/2 - fontSize/2, fontSize, WHITE);
        };

        DrawFancyButton(playButton, "Play");
        DrawFancyButton(charactersButton, "Characters");
        DrawFancyButton(optionsButton, "Options");
        DrawFancyButton(quitButton, "Quit");
    }

    // ===== CHARACTERS MENU =====
    else if (state == MenuState::Characters) {
        DrawRectangleRec(backButton, DARKGRAY);
        if(CheckCollisionPointRec(GetMousePosition(), backButton))
            DrawRectangleLinesEx(backButton, 3, WHITE);
        DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);

        int columns = 4;
        float spacing = 30;
        float cardWidth = (screenWidth - 200 - spacing*(columns-1))/columns;
        float cardHeight = 220;
        float startX = 100;
        float startY = 120 - scrollOffset;

        for (int i = 0; i < characters.size(); i++) {
            int row = i / columns;
            int col = i % columns;

            Rectangle card = {
                startX + col*(cardWidth+spacing),
                startY + row*(cardHeight+spacing),
                cardWidth,
                cardHeight
            };

            if(card.y + card.height < 0 || card.y > screenHeight) continue;

            DrawRectangleRec(card, DARKGRAY);
            bool hover = CheckCollisionPointRec(GetMousePosition(), card);
            if(hover) DrawRectangleLinesEx(card, 2, LIGHTGRAY);
            if(i == selectedCharacter) DrawRectangleLinesEx(card, 4, YELLOW);

            Rectangle imageRect = { card.x+20, card.y+20, card.width-40, 100 };
            DrawRectangleRec(imageRect, characters[i].color);

            int nameSize = 20;
            int nameWidth = MeasureText(characters[i].name.c_str(), nameSize);
            DrawText(characters[i].name.c_str(),
                     card.x + card.width/2 - nameWidth/2,
                     card.y + 130,
                     nameSize,
                     WHITE);

            int descSize = 16;
            DrawText(characters[i].description.c_str(),
                     card.x + 20,
                     card.y + 160,
                     descSize,
                     GRAY);

            if(hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                selectedCharacter = i;
        }
    }

    // ===== OPTIONS MENU =====
    else if (state == MenuState::Options) {
        DrawRectangleRec(backButton, DARKGRAY);
        if(CheckCollisionPointRec(GetMousePosition(), backButton))
            DrawRectangleLinesEx(backButton, 3, WHITE);
        DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);

        Rectangle panel = { 80, 80, screenWidth-160, screenHeight-160 };
        DrawRectangleRounded(panel, 0.02f, 8, Color{30,30,40,230});
        DrawRectangleRoundedLines(panel, 0.02f, 8, WHITE);

        float baseX = panel.x + 80;
        float baseY = panel.y + 80;
        DrawText("Settings", baseX, panel.y+30, 40, WHITE);

        // Fullscreen toggle
        Rectangle toggle = { baseX, baseY, 70, 30 };
        DrawText("Fullscreen", toggle.x + 110, toggle.y + 5, 20, WHITE);
        DrawRectangleRounded(toggle, 1.0f, 10, fullscreen ? GREEN : DARKGRAY);
        float circleX = fullscreen ? toggle.x+40 : toggle.x+5;
        DrawCircle(circleX+10, toggle.y+15, 12, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), toggle) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            fullscreen = !fullscreen;
            ToggleFullscreen();
        }

        // Mouse sensitivity
        Rectangle sensBar = { baseX, baseY+100, 350, 8 };
        DrawText("Mouse Sensitivity", sensBar.x, sensBar.y-30, 20, WHITE);
        DrawRectangleRounded(sensBar, 1.0f, 10, DARKGRAY);
        float fillWidth = (mouseSensitivity/5.0f)*sensBar.width;
        DrawRectangleRounded({sensBar.x, sensBar.y, fillWidth, sensBar.height},1.0f,10,SKYBLUE);
        DrawCircle(sensBar.x+fillWidth, sensBar.y+sensBar.height/2, 10, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), {sensBar.x, sensBar.y-10, sensBar.width, 30}) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float ratio = (GetMouseX() - sensBar.x)/sensBar.width;
            mouseSensitivity = ratio*5.0f;
            if(mouseSensitivity<0.1f) mouseSensitivity=0.1f;
            if(mouseSensitivity>5.0f) mouseSensitivity=5.0f;
        }

        // AimMode dropdown
        const char* aimModes[] = {"Closest Enemy","Mouse Position"};
        static bool dropdownOpen=false;
        Rectangle dropdownRect = { baseX, baseY+190, 260, 40 };
        DrawText("Aim Mode", dropdownRect.x, dropdownRect.y-30, 20, WHITE);
        DrawRectangleRounded(dropdownRect,0.2f,6,Color{45,45,60,255});
        DrawRectangleRoundedLines(dropdownRect,0.2f,6,WHITE);
        DrawText(aimModes[(int)aimMode], dropdownRect.x+10, dropdownRect.y+10, 20, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), dropdownRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            dropdownOpen = !dropdownOpen;

        if(dropdownOpen) {
            for(int i=0;i<2;i++) {
                Rectangle optRect = { dropdownRect.x, dropdownRect.y+(i+1)*dropdownRect.height+5*i,
                                      dropdownRect.width, dropdownRect.height };
                bool hover = CheckCollisionPointRec(GetMousePosition(), optRect);
                Color bgColor = Color{35,35,50,255};
                if(i==(int)aimMode) bgColor = Color{70,130,200,255};
                else if(hover) bgColor = Color{60,60,80,255};
                DrawRectangleRounded(optRect,0.2f,6,bgColor);
                DrawText(aimModes[i], optRect.x+10, optRect.y+10, 20, WHITE);
                if(hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    aimMode = (AimMode)i;
                    dropdownOpen=false;
                }
            }
        }
    }
}

bool MainMenu::ShouldStartGame() const { return startGame; }
bool MainMenu::ShouldQuit() const { return quitGame; }
void MainMenu::ResetFlags() { startGame=false; }
int MainMenu::GetSelectedCharacter() const { return selectedCharacter; }
bool MainMenu::ButtonLogic(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(),rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}