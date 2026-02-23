#pragma once
#include "raylib.h"
#include <vector>
#include <string>

enum class MenuState
{
    Main,
    Options,
    Characters,
    None
};

enum class AimMode
{
    ClosestEnemy,
    MousePosition
};

struct CharacterData
{
    std::string name;
    std::string description;
    Color color;
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

    int GetSelectedCharacter() const;
    
    bool fullscreen;
    float mouseSensitivity;
    AimMode aimMode;

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

    std::vector<CharacterData> characters;
    int selectedCharacter;

    float scrollOffset;
    float contentHeight;

    void DrawButton(Rectangle rect, const char* text);
    bool ButtonLogic(Rectangle rect);
};
