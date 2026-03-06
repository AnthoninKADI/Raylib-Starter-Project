#include "PauseMenu.h"
#include <string>

#include "MainMenu.h"

PauseMenu::PauseMenu(float screenW, float screenH)
{
    screenWidth = screenW;
    screenHeight = screenH;

    float btnW = 300;
    float btnH = 70;
    float spacing = 30;
    float centerX = screenWidth/2 - btnW/2;
    float startY = screenHeight/2 - (btnH*3 + spacing*2)/2;

    playButton     = { centerX, startY, btnW, btnH };
    settingsButton = { centerX, startY + btnH + spacing, btnW, btnH };
    backButton     = { centerX, startY + (btnH + spacing)*2, btnW, btnH };

    isPaused = false;
    resumeRequested = false;
    backToMenuRequested = false;
    settingsRequested = false;
}

void PauseMenu::Toggle()
{
    isPaused = !isPaused;
    resumeRequested = false;
    backToMenuRequested = false;
    settingsRequested = false;
}

void PauseMenu::Update()
{
    if(!isPaused) return;

    Vector2 mouse = GetMousePosition();
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if(CheckCollisionPointRec(mouse, playButton))
            resumeRequested = true;
        else if(CheckCollisionPointRec(mouse, settingsButton))
            settingsRequested = true;
        else if(CheckCollisionPointRec(mouse, backButton))
            backToMenuRequested = true;
    }
}

void PauseMenu::DrawButton(const Rectangle& rect, const char* text)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), rect);

    float scale = hover ? 1.08f : 1.0f;

    Rectangle scaled = {
        rect.x - (rect.width * (scale - 1) / 2),
        rect.y - (rect.height * (scale - 1) / 2),
        rect.width * scale,
        rect.height * scale
    };

    Color base = {40,40,60,255};
    Color hoverCol = {60,60,90,255};

    DrawRectangleRounded(scaled, 0.3f, 8, hover ? hoverCol : base);

    if (hover)
        DrawRectangleRoundedLines(scaled, 0.3f, 8, WHITE);

    int fontSize = 30;
    int textWidth = MeasureText(text, fontSize);

    DrawText(text,
        scaled.x + scaled.width/2 - textWidth/2,
        scaled.y + scaled.height/2 - fontSize/2,
        fontSize,
        WHITE);
}

void PauseMenu::Draw()
{
    if(!isPaused) return;

    for(int y=0;y<screenHeight;y++){
        float t = (float)y/screenHeight;
        Color col = {
            (unsigned char)(10 + 20*t),
            (unsigned char)(10 + 20*t),
            (unsigned char)(30 + 60*t),
            255
        };
        DrawLine(0,y,screenWidth,y,col);
    }
    
    DrawRectangle(0,0,screenWidth,screenHeight,Color{0,0,0,150});

    const char* title = "GAME PAUSED";
    int fontSize = 70;
    int textWidth = MeasureText(title, fontSize);

    DrawText(title,
             screenWidth/2 - textWidth/2,
             screenHeight/6,
             fontSize,
             WHITE);
    
    DrawButton(playButton,"Play");
    DrawButton(settingsButton,"Settings");
    DrawButton(backButton,"Back to Menu");
}

void DrawSettingsMenu(float screenWidth, float screenHeight, bool &showSettings, bool &fullscreen, float &mouseSensitivity, MainMenu::AimMode &aimMode)
{
    // Fond semi-transparent
    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    Rectangle panel = { 80, 80, screenWidth-160, screenHeight-160 };
    DrawRectangleRounded(panel, 0.02f, 8, Color{30,30,40,200});
    DrawRectangleRoundedLines(panel, 0.02f, 8, WHITE);

    DrawText("Settings", panel.x + 80, panel.y + 30, 40, WHITE);

    // Bouton Back
    Rectangle backButton = { panel.x + 20, panel.y + 20, 40, 40 };
    DrawRectangleRec(backButton, DARKGRAY);

    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, backButton);

    if(hover)
    {
        DrawRectangleLinesEx(backButton, 3, WHITE);

        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            printf("Settings closed\n");
            showSettings = false;
        }
    }

    DrawText("<", backButton.x + 12, backButton.y + 5, 30, WHITE);
    

    float baseX = panel.x + 80;
    float baseY = panel.y + 80;

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

    // Sensibilité souris
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

    // Aim Mode dropdown
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
                aimMode = (MainMenu::AimMode)i;
                dropdownOpen=false;
            }
        }
    }
}
void PauseMenu::ClearSettingsRequest()
{
    settingsRequested = false;
}