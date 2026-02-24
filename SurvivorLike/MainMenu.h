#pragma once
#include "raylib.h"
#include <vector>
#include <string>

class MainMenu
{
public:

    enum class MenuState {
        Main,
        Characters,
        Options
    };

    enum class AimMode {
        ClosestEnemy,
        MousePosition
    };

    MainMenu(float sw, float sh);

    void Update();
    void Draw();

    bool ShouldStartGame() const;
    bool ShouldQuit() const;
    void ResetFlags();

    int GetSelectedCharacter() const;

private:

    struct Character {
        std::string name;
        std::string description;
        Color color;
    };

    struct Particle {
        float x;
        float y;
        float speed;
        float size;
    };

    void DrawFancyButton(Rectangle rect, const char* text);
    bool ButtonLogic(Rectangle rect);

    // ===== SCREEN =====
    float screenWidth;
    float screenHeight;

    // ===== STATE =====
    MenuState state;
    bool startGame;
    bool quitGame;

    // ===== BUTTONS =====
    Rectangle playButton;
    Rectangle charactersButton;
    Rectangle optionsButton;
    Rectangle quitButton;
    Rectangle backButton;

    // ===== CHARACTERS =====
    std::vector<Character> characters;
    int selectedCharacter;
    float scrollOffset;
    float contentHeight;

    // ===== OPTIONS =====
    bool fullscreen;
    float mouseSensitivity;
    AimMode aimMode;

    // ===== VISUAL EFFECTS =====
    float titlePulse;
    std::vector<Particle> particles;
};