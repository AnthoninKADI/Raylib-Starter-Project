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

    struct Stats {
        float speed;
        float lifesteal;
        float damage;
        float attackCooldown;
        float maxHP;
        int shots;
        float effectDuration;
        int enemiesPierced;
    };

    MainMenu(float sw, float sh);

    void Update();
    void Draw();

    bool ShouldStartGame() const;
    bool ShouldQuit() const;
    void ResetFlags();

    int GetSelectedCharacter() const;
    const Stats& GetSelectedStats() const;
    Texture2D GetSelectedTexture() const;
    const std::string& GetSelectedTitle() const;
    const std::string& GetSelectedDescription() const;

private:

    struct Character {
        std::string name;
        std::string description;
        Texture2D texture;
        Stats stats;
    };

    struct Particle {
        float x;
        float y;
        float speed;
        float size;
    };

    void DrawFancyButton(Rectangle rect, const char* text);
    bool ButtonLogic(Rectangle rect);

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

    std::vector<Character> characters;
    int selectedCharacter;
    float scrollOffset;
    float contentHeight;

    bool fullscreen;
    float mouseSensitivity;
    AimMode aimMode;

    float titlePulse;
    std::vector<Particle> particles;
    
public:
    AimMode GetAimMode() const;
};