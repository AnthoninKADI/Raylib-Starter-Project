#pragma once
#include "raylib.h"

enum class MenuState
{
    Main,
    Options,
    Characters,
    None
};

class MainMenu
{
public:
    MainMenu(float screenWidth, float screenHeight);

    void Update();
    void Draw();

    bool ShouldStartGame() const;
    bool ShouldQuit() const;

    void ResetFlags();

private:
    float screenWidth;
    float screenHeight;

    MenuState state;

    bool startGame;
    bool quitGame;

    Rectangle playButton;
    Rectangle charactersButton;
    Rectangle optionsButton;
    Rectangle quitButton;

    Rectangle backButton;

    void DrawButton(Rectangle rect, const char* text);
    bool ButtonLogic(Rectangle rect);
};
