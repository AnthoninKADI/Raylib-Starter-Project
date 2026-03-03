#include "MainMenu.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

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
        { "Archer", "Balanced fighter", LoadTexture("assets/textures/Archer.png"),
          {400.0f, 0.0f, 10.0f, 1.5f, 100.0f, 1, 0.0f, 1} },
        { "Warrior", "High HP and damage", LoadTexture("assets/textures/AuraPlayer.png"),
          {300.0f, 0.0f, 20.0f, 2.0f, 200.0f, 1, 0.0f, 1} },
        { "Rogue", "Fast and agile", LoadTexture("assets/textures/AxePlayer.png"),
          {500.0f, 0.0f, 8.0f, 1.0f, 80.0f, 1, 0.0f, 1} },
        { "Mage", "Strong ranged attacks", LoadTexture("assets/textures/Mage.png"),
          {350.0f, 0.0f, 15.0f, 1.2f, 70.0f, 1, 5.0f, 1} },
        { "Electric", "Strong ranged attacks", LoadTexture("assets/textures/LightningMage.png"),
                {350.0f, 0.0f, 15.0f, 1.2f, 70.0f, 1, 5.0f, 1} }
    };

    selectedCharacter = 0;
    scrollOffset = 0.0f;
    contentHeight = 0.0f;

    fullscreen = false;
    mouseSensitivity = 1.0f;
    aimMode = AimMode::ClosestEnemy;

    titlePulse = 0.0f;

    srand((unsigned int)time(NULL));

    for(int i=0;i<80;i++){
        Particle p;
        p.x = rand()%((int)screenWidth);
        p.y = rand()%((int)screenHeight);
        p.speed = 20 + rand()%40;
        p.size = 1 + rand()%3;
        particles.push_back(p);
    }
}

const MainMenu::Stats& MainMenu::GetSelectedStats() const
{
    return characters[selectedCharacter].stats;
}

Texture2D MainMenu::GetSelectedTexture() const
{
    return characters[selectedCharacter].texture;
}

const std::string& MainMenu::GetSelectedTitle() const
{
    return characters[selectedCharacter].name;
}

const std::string& MainMenu::GetSelectedDescription() const
{
    return characters[selectedCharacter].description;
}

void MainMenu::Update()
{
    float dt = GetFrameTime();
    titlePulse += dt * 3.0f;

    for(auto &p : particles){
        p.y += p.speed * dt;
        if(p.y > screenHeight){
            p.y = 0;
            p.x = rand()%((int)screenWidth);
        }
    }

    if(state == MenuState::Main){
        if(ButtonLogic(playButton)) startGame = true;
        if(ButtonLogic(charactersButton)) state = MenuState::Characters;
        if(ButtonLogic(optionsButton)) state = MenuState::Options;
        if(ButtonLogic(quitButton)) quitGame = true;
    }
    else if(state == MenuState::Characters)
    {
        if(CheckCollisionPointRec(GetMousePosition(), backButton)
           && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            state = MenuState::Main;

        float wheel = GetMouseWheelMove();
        scrollOffset += -wheel * 40.0f;

        float cardHeight = 180;
        float spacing = 25;
        contentHeight = characters.size() * cardHeight + (characters.size() - 1) * spacing;
        float panelHeight = screenHeight - 160;  
        float visibleHeight = panelHeight - 130; 
        float maxScroll = contentHeight - visibleHeight;
        if(maxScroll < 0) maxScroll = 0;
        if(scrollOffset < 0) scrollOffset = 0;
        if(scrollOffset > maxScroll) scrollOffset = maxScroll;
    }
    else{
        if(CheckCollisionPointRec(GetMousePosition(), backButton)
           && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            state = MenuState::Main;
    }
}

void MainMenu::Draw()
{
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

    for(auto &p : particles){
        DrawCircle(p.x,p.y,p.size,Color{200,200,255,150});
    }

    if(state == MenuState::Main){
        const char* title = "VAMPIRE SURVIVOR";
        float pulse = 1.0f + 0.05f * sin(titlePulse * 2.5f);
        int fontSize = (int)(80 * pulse);
        int textWidth = MeasureText(title,fontSize);

        DrawText(title, screenWidth/2 - textWidth/2, 140, fontSize, WHITE);

        DrawFancyButton(playButton,"Play");
        DrawFancyButton(charactersButton,"Characters");
        DrawFancyButton(optionsButton,"Options");
        DrawFancyButton(quitButton,"Quit");
    }
    else if(state == MenuState::Characters)
    {
        Rectangle panel = {80,80,screenWidth-160,screenHeight-160};
        DrawRectangleRounded(panel,0.02f,8,Color{25,25,35,240});
        DrawRectangleRoundedLines(panel,0.02f,8,Color{80,80,120,255});
        DrawRectangleRec(backButton,DARKGRAY);
        if(CheckCollisionPointRec(GetMousePosition(),backButton))
            DrawRectangleLinesEx(backButton,3,WHITE);
        DrawText("<",backButton.x+12,backButton.y+5,30,WHITE);
        DrawText("Choose Your Character", panel.x+40, panel.y+30, 35, WHITE);

        Rectangle scrollArea = { panel.x+40, panel.y+90, panel.width-80, panel.height-130 };
        BeginScissorMode((int)scrollArea.x,(int)scrollArea.y,(int)scrollArea.width,(int)scrollArea.height);

        float startX = scrollArea.x + 20;
        float startY = scrollArea.y - scrollOffset;
        float cardWidth = scrollArea.width - 40;
        float cardHeight = 180;
        float spacing = 25;

        for(int i=0;i<characters.size();i++)
        {
            Rectangle card = { startX, startY + i*(cardHeight+spacing), cardWidth, cardHeight };
            bool hover = CheckCollisionPointRec(GetMousePosition(),card);
            Color bg = Color{35,35,50,255};
            if(i==selectedCharacter) bg = Color{70,120,200,255};
            else if(hover) bg = Color{55,55,80,255};
            DrawRectangleRounded(card,0.15f,10,bg);
            if(i==selectedCharacter)
                DrawRectangleRoundedLines(card,0.15f,10,YELLOW);
            else if(hover)
                DrawRectangleRoundedLines(card,0.15f,10,LIGHTGRAY);

            Rectangle avatar = {card.x+20,card.y+20,180,180};
            DrawTexturePro(characters[i].texture,
                {0,0,(float)characters[i].texture.width,(float)characters[i].texture.height},
                {avatar.x,avatar.y,avatar.width,avatar.height},
                {0,0},0,WHITE);

            DrawText(characters[i].name.c_str(), card.x+220, card.y+25, 24, WHITE);
            DrawText(characters[i].description.c_str(), card.x+220, card.y+60, 18, LIGHTGRAY);

            if(hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                selectedCharacter = i;
        }

        EndScissorMode();
    }
    else if(state == MenuState::Options){
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

        Rectangle toggle = { baseX, baseY, 70, 30 };
        DrawText("Fullscreen", toggle.x + 110, toggle.y + 5, 20, WHITE);
        DrawRectangleRounded(toggle, 1.0f, 10, fullscreen ? GREEN : DARKGRAY);
        float circleX = fullscreen ? toggle.x+40 : toggle.x+5;
        DrawCircle(circleX+10, toggle.y+15, 12, WHITE);
        if(CheckCollisionPointRec(GetMousePosition(), toggle) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            fullscreen = !fullscreen;
            ToggleFullscreen();
        }

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

void MainMenu::DrawFancyButton(Rectangle rect,const char* text)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(),rect);
    float scale = hover ? 1.08f : 1.0f;
    Rectangle scaled = { rect.x - (rect.width*(scale-1)/2), rect.y - (rect.height*(scale-1)/2), rect.width*scale, rect.height*scale };
    DrawRectangleRounded(scaled,0.3f,8, hover?Color{60,60,90,255}:Color{40,40,60,255});
    if(hover) DrawRectangleRoundedLines(scaled,0.3f,8,WHITE);
    int fontSize=30;
    int textWidth=MeasureText(text,fontSize);
    DrawText(text, scaled.x+scaled.width/2-textWidth/2, scaled.y+scaled.height/2-fontSize/2, fontSize, WHITE);
}

bool MainMenu::ButtonLogic(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(),rect)
        && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool MainMenu::ShouldStartGame() const { return startGame; }
bool MainMenu::ShouldQuit() const { return quitGame; }
void MainMenu::ResetFlags(){ startGame=false; }
int MainMenu::GetSelectedCharacter() const { return selectedCharacter; }