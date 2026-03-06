#pragma once
#include <vector>

#include "raylib.h"

class PauseMenu
{
public:
    PauseMenu(float screenW, float screenH);
    
    void Update();
    void Draw();
    void Toggle();
    bool IsPaused() const { return isPaused; }
    bool ResumeRequested() const { return resumeRequested; }
    bool BackToMenuRequested() const { return backToMenuRequested; }
    bool SettingsRequested() const { return settingsRequested; }
    void ClearSettingsRequest();

private:
    float screenWidth;
    float screenHeight;

    Rectangle playButton;
    Rectangle settingsButton;
    Rectangle backButton;

    bool isPaused;
    bool resumeRequested;
    bool backToMenuRequested;
    bool settingsRequested;

    void DrawButton(const Rectangle& rect, const char* text);
};