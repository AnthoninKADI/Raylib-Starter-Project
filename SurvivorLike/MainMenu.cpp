#include "MainMenu.h"
#include <algorithm>

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
}

void MainMenu::Update()
{
    if(state == MenuState::Main)
    {
        if(ButtonLogic(playButton)) startGame = true;
        if(ButtonLogic(charactersButton)) state = MenuState::Characters;
        if(ButtonLogic(optionsButton)) state = MenuState::Options;
        if(ButtonLogic(quitButton)) quitGame = true;
    }
    else if(state == MenuState::Characters)
    {
        if(CheckCollisionPointRec(GetMousePosition(), backButton))
        {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                state = MenuState::Main;
        }

        float wheel = GetMouseWheelMove();
        scrollOffset -= wheel * 40.0f;
        scrollOffset = std::max(0.0f, std::min(scrollOffset, std::max(0.0f, contentHeight - screenHeight + 120.0f)));

        int columns = 4;
        float spacing = 30;
        float cardWidth = (screenWidth - 200 - spacing * (columns - 1)) / columns;
        float cardHeight = 220;
        float startX = 100;
        float startY = 120 - scrollOffset;

        for(int i = 0; i < characters.size(); i++)
        {
            int row = i / columns;
            int col = i % columns;

            Rectangle card = {
                startX + col * (cardWidth + spacing),
                startY + row * (cardHeight + spacing),
                cardWidth,
                cardHeight
            };

            if(CheckCollisionPointRec(GetMousePosition(), card))
            {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    selectedCharacter = i;
            }
        }

        int rows = (characters.size() + columns - 1) / columns;
        contentHeight = rows * (cardHeight + spacing);
    }
    else
    {
        if(CheckCollisionPointRec(GetMousePosition(), backButton))
        {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                state = MenuState::Main;
        }
    }
}

void MainMenu::Draw()
{
    if(state == MenuState::Main)
    {
        const char* title = "VAMPIRE SURVIVOR";
        int fontSize = 80;
        int textWidth = MeasureText(title, fontSize);
        DrawText(title, screenWidth/2 - textWidth/2, 150, fontSize, WHITE);

        DrawButton(playButton, "Play");
        DrawButton(charactersButton, "Characters");
        DrawButton(optionsButton, "Options");
        DrawButton(quitButton, "Quit");
    }
   else if(state == MenuState::Options)
    {
    DrawRectangleRec(backButton, DARKGRAY);
    bool hoverBack = CheckCollisionPointRec(GetMousePosition(), backButton);
    if(hoverBack) DrawRectangleLines(backButton.x, backButton.y, backButton.width, backButton.height, WHITE);
    DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);

    const char* title = "Options";
    int fontSize = 50;
    int textWidth = MeasureText(title, fontSize);
    DrawText(title, 100 + backButton.width + 10, 40, fontSize, WHITE); // décalé à droite de la flèche

    // Fullscreen toggle
    Rectangle fsRect = { 100, 150, 40, 40 };
    DrawRectangleRec(fsRect, DARKGRAY);
    if(fullscreen) DrawRectangleRec({fsRect.x+5, fsRect.y+5, 30, 30}, GREEN);
    bool hoverFS = CheckCollisionPointRec(GetMousePosition(), fsRect);
    if(hoverFS) DrawRectangleLines(fsRect.x, fsRect.y, fsRect.width, fsRect.height, WHITE);
    DrawText("Fullscreen", fsRect.x + 60, fsRect.y + 5, 20, WHITE);
    if(hoverFS && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        fullscreen = !fullscreen;
        ToggleFullscreen();
    }

    // Mouse sensitivity slider
    Rectangle sensBar = { 100, 220, 300, 20 };
    DrawText("Mouse Sensitivity", sensBar.x, sensBar.y - 25, 20, WHITE);
    DrawRectangleRec(sensBar, DARKGRAY);
    float handleX = sensBar.x + (mouseSensitivity / 5.0f) * sensBar.width - 5;
    DrawRectangle(handleX, sensBar.y - 5, 10, 30, SKYBLUE);
    if(CheckCollisionPointRec(GetMousePosition(), sensBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        float mx = GetMouseX();
        float ratio = (mx - sensBar.x) / sensBar.width;
        mouseSensitivity = ratio * 5.0f;
        if(mouseSensitivity < 0.1f) mouseSensitivity = 0.1f;
        if(mouseSensitivity > 5.0f) mouseSensitivity = 5.0f;

    }
       
    const char* aimModes[] = { "Closest Enemy", "Mouse Position" };
    static bool dropdownOpen = false;
    Rectangle dropdownRect = { 100, 270, 200, 30 };
    DrawText("Aim Mode", dropdownRect.x, dropdownRect.y - 25, 20, WHITE);
    DrawRectangleRec(dropdownRect, DARKGRAY);
    DrawText(aimModes[(int)aimMode], dropdownRect.x + 10, dropdownRect.y + 5, 20, WHITE);
    if(CheckCollisionPointRec(GetMousePosition(), dropdownRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        dropdownOpen = !dropdownOpen;

    if(dropdownOpen)
    {
        for(int i = 0; i < 2; i++)
        {
            Rectangle optRect = { dropdownRect.x, dropdownRect.y + (i+1)*dropdownRect.height, dropdownRect.width, dropdownRect.height };
            DrawRectangleRec(optRect, DARKGRAY);
            bool hover = CheckCollisionPointRec(GetMousePosition(), optRect);
            if(hover) DrawRectangleLines(optRect.x, optRect.y, optRect.width, optRect.height, WHITE);
            DrawText(aimModes[i], optRect.x + 10, optRect.y + 5, 20, WHITE);
            if(hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                aimMode = (AimMode)i;
                dropdownOpen = false;
            }
        }
    }
}


    else if(state == MenuState::Characters)
    {
        DrawRectangleRec(backButton, DARKGRAY);
        bool hoverBack = CheckCollisionPointRec(GetMousePosition(), backButton);
        if(hoverBack) DrawRectangleLinesEx(backButton, 3, WHITE);
        DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);

        int columns = 4;
        float spacing = 30;
        float cardWidth = (screenWidth - 200 - spacing * (columns - 1)) / columns;
        float cardHeight = 220;
        float startX = 100;
        float startY = 120 - scrollOffset;

        for(int i = 0; i < characters.size(); i++)
        {
            int row = i / columns;
            int col = i % columns;

            Rectangle card = {
                startX + col * (cardWidth + spacing),
                startY + row * (cardHeight + spacing),
                cardWidth,
                cardHeight
            };

            if(card.y + card.height < 0 || card.y > screenHeight)
                continue;

            DrawRectangleRec(card, DARKGRAY);

            bool hover = CheckCollisionPointRec(GetMousePosition(), card);
            if(hover) DrawRectangleLinesEx(card, 2, LIGHTGRAY);

            if(i == selectedCharacter)
                DrawRectangleLinesEx(card, 4, YELLOW);

            Rectangle imageRect = { card.x + 20, card.y + 20, card.width - 40, 100 };
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
        }
    }
}

bool MainMenu::ShouldStartGame() const
{
    return startGame;
}

bool MainMenu::ShouldQuit() const
{
    return quitGame;
}

void MainMenu::ResetFlags()
{
    startGame = false;
}

int MainMenu::GetSelectedCharacter() const
{
    return selectedCharacter;
}

void MainMenu::DrawButton(Rectangle rect, const char* text)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), rect);

    DrawRectangleRec(rect, hover ? GRAY : DARKGRAY);
    if(hover) DrawRectangleLinesEx(rect, 3, WHITE);

    int fontSize = 30;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text,
             rect.x + rect.width/2 - textWidth/2,
             rect.y + rect.height/2 - fontSize/2,
             fontSize,
             WHITE);
}

bool MainMenu::ButtonLogic(Rectangle rect)
{
    if(CheckCollisionPointRec(GetMousePosition(), rect))
    {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            return true;
    }
    return false;
}
