#include "MainMenu.h"

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
        const char* text = "Options - Coming Soon";
        int fontSize = 40;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, screenWidth/2 - textWidth/2, screenHeight/2 - 20, fontSize, WHITE);

        DrawRectangleRec(backButton, DARKGRAY);

        bool hover = CheckCollisionPointRec(GetMousePosition(), backButton);
        if(hover) DrawRectangleLinesEx(backButton, 3, WHITE);

        DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);
    }
    else if(state == MenuState::Characters)
    {
        const char* text = "Characters - Coming Soon";
        int fontSize = 40;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, screenWidth/2 - textWidth/2, screenHeight/2 - 20, fontSize, WHITE);

        DrawRectangleRec(backButton, DARKGRAY);

        bool hover = CheckCollisionPointRec(GetMousePosition(), backButton);
        if(hover) DrawRectangleLinesEx(backButton, 3, WHITE);

        DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);
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
