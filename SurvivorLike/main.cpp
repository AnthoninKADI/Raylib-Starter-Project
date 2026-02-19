#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "DebugMenu.h"
#include "Entities.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

const int screenWidth  = 1600;
const int screenHeight = 900;
const float menuWidth  = 350.0f;

float tileSize = 80.0f;
const int mapWidth  = 200;
const int mapHeight = 200;

Vector2 playerPos;
float playerSpeed = 400.0f;
float playerSize  = 80.0f;
int playerLevel   = 1;
float playerXP    = 0;
float xpToLevel   = 100;
float xpOrbValue  = 10.0f;
float levelUpDuration = 1.0f; 

float playerMaxHP = 100.0f;
float playerHP    = 100.0f;
float playerInvincibilityTimer = 0.0f;

int totalKills = 0;

struct LevelUpText {
    Vector2 pos;
    float timer;
};
std::vector<LevelUpText> levelUpTexts;

std::vector<Enemy> enemies;
float enemySpawnInterval = 2.0f;
float enemySpawnTimer    = 0.0f;
float enemySpeed         = 150.0f;
float enemySize          = 60.0f;
bool spawnOnClick        = false;

std::vector<XPOrb> xpOrbs;

struct Tile { Vector2 pos; Texture2D texture; };
std::vector<Tile> mapTiles;

float gameTime = 0.0f;
bool killAllEnemiesFlag = false;

float CalculateXPForLevel(int level)
{
    if(level <= 1) return 100.0f;

    float base = 100.0f;
    for(int i = 2; i <= level; i++)
    {
        if(i < 10)        base += 35.0f;
        else if(i < 20)   base += 55.0f;
        else if(i < 40)   base += 80.0f;
        else if(i < 60)   base += 110.0f;
        else              base += 150.0f;
    }
    return base;
}

std::string FormatTime(float seconds)
{
    int h = (int)(seconds/3600);
    int m = ((int)seconds%3600)/60;
    int s = (int)seconds%60;

    char buffer[32];
    if(h>0) sprintf_s(buffer,sizeof(buffer),"%dh%02dm%02ds",h,m,s);
    else if(m>0) sprintf_s(buffer,sizeof(buffer),"%dm%02ds",m,s);
    else sprintf_s(buffer,sizeof(buffer),"%ds",s);

    return std::string(buffer);
}

std::string FormatXP(float xp,float xpNext)
{
    char buf[32];
    sprintf_s(buf,sizeof(buf),"%.0f / %.0f",xp,xpNext);
    return std::string(buf);
}

void DrawTextOutlined(Font font, const std::string& text, Vector2 pos, float fontSize, float spacing, Color color)
{
    DrawTextEx(font, text.c_str(), {pos.x-2, pos.y}, fontSize, spacing, BLACK);
    DrawTextEx(font, text.c_str(), {pos.x+2, pos.y}, fontSize, spacing, BLACK);
    DrawTextEx(font, text.c_str(), {pos.x, pos.y-2}, fontSize, spacing, BLACK);
    DrawTextEx(font, text.c_str(), {pos.x, pos.y+2}, fontSize, spacing, BLACK);
    DrawTextEx(font, text.c_str(), pos, fontSize, spacing, color);
}

int main()
{
    InitWindow(screenWidth, screenHeight,"Vampire Survivor Base");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    Font gameFont = LoadFontEx("assets/font/Nordhin.ttf",64,0,0);
    SetTextureFilter(gameFont.texture,TEXTURE_FILTER_BILINEAR);

    Texture2D texGrass  = LoadTexture("assets/textures/Grass.png");
    Texture2D texPlayer = LoadTexture("assets/textures/PlayerSlime.png");
    Texture2D texEnemy  = LoadTexture("assets/textures/EnemySlime.png");
    Texture2D texXP     = LoadTexture("assets/textures/XP.png");
    Texture2D texSkull  = LoadTexture("assets/textures/skull.png");

    for(int x=0;x<mapWidth;x++)
        for(int y=0;y<mapHeight;y++)
            mapTiles.push_back({ {x*tileSize,y*tileSize}, texGrass });

    playerPos = { mapWidth*tileSize/2.0f, mapHeight*tileSize/2.0f };

    Camera2D camera = {0};
    camera.target   = playerPos;
    camera.offset   = { (screenWidth-menuWidth)/2.0f, screenHeight/2.0f };
    camera.zoom     = 1.0f;

    DebugMenu debugMenu(menuWidth);

    const float hitCooldown = 0.5f;
    const float passiveRegen = 1.0f;
    const float flashSpeed = 15.0f;

    int previousLevel = playerLevel;

    while(!WindowShouldClose())
    {
        float delta = GetFrameTime();
        gameTime += delta;

        previousLevel = playerLevel;

        if(playerInvincibilityTimer > 0) playerInvincibilityTimer -= delta;
        else playerInvincibilityTimer = 0;

        if(playerHP < playerMaxHP && playerInvincibilityTimer <= 0)
            playerHP = std::min(playerMaxHP, playerHP + passiveRegen*delta);

        Vector2 dir = {0,0};
        if(IsKeyDown(KEY_W)) dir.y -= 1;
        if(IsKeyDown(KEY_S)) dir.y += 1;
        if(IsKeyDown(KEY_A)) dir.x -= 1;
        if(IsKeyDown(KEY_D)) dir.x += 1;

        float len = sqrt(dir.x*dir.x + dir.y*dir.y);
        if(len>0){ dir.x/=len; dir.y/=len; }

        playerPos.x += dir.x*playerSpeed*delta;
        playerPos.y += dir.y*playerSpeed*delta;

        camera.target = playerPos;
        
        enemySpawnTimer += delta;
        if(enemySpawnTimer >= enemySpawnInterval)
        {
            enemySpawnTimer = 0.0f;
            float angle = GetRandomValue(0,359) * DEG2RAD;
            float radius = 300;

            Enemy e;
            e.pos = { playerPos.x + cos(angle)*radius,
                      playerPos.y + sin(angle)*radius };
            e.size = enemySize;
            e.speed = enemySpeed;
            e.texture = texEnemy;
            enemies.push_back(e);
        }
        
        for(auto &e : enemies)
        {
            Vector2 d = { playerPos.x - e.pos.x,
                          playerPos.y - e.pos.y };
            float l = sqrt(d.x*d.x + d.y*d.y);
            if(l>0){ d.x/=l; d.y/=l; }
            e.pos.x += d.x*e.speed*delta;
            e.pos.y += d.y*e.speed*delta;

            float hitRadius = (playerSize + e.size) * 0.5f;
            if(l < hitRadius && playerInvincibilityTimer <= 0)
            {
                playerHP -= 10.0f;
                if(playerHP < 0) playerHP = 0;
                playerInvincibilityTimer = hitCooldown;
            }
        }
        
        for (int i = 0; i < xpOrbs.size(); )
        {
            float dx = playerPos.x - xpOrbs[i].pos.x;
            float dy = playerPos.y - xpOrbs[i].pos.y;
            float dist = sqrt(dx*dx + dy*dy);

            float pickupRadius = playerSize * 0.6f;

            if (dist < pickupRadius)
            {
                playerXP += xpOrbValue;
                xpOrbs.erase(xpOrbs.begin() + i);
            }
            else i++;
        }
        
        xpToLevel = CalculateXPForLevel(playerLevel);
        
        while(playerXP >= xpToLevel)
        {
            playerXP -= xpToLevel;
            playerLevel++;
            xpToLevel = CalculateXPForLevel(playerLevel);

            LevelUpText t;
            t.pos = { playerPos.x, playerPos.y - playerSize - 20 };
            t.timer = levelUpDuration;
            levelUpTexts.push_back(t);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        
        for(auto &tile : mapTiles)
            DrawTexturePro(tile.texture,
                {0,0,(float)tile.texture.width,(float)tile.texture.height},
                {tile.pos.x,tile.pos.y,tileSize,tileSize},
                {0,0},0,WHITE);
        
        float pScale = playerSize/texPlayer.width;
        Color drawColor = WHITE;
        if(playerInvincibilityTimer > 0)
        {
            int flashPhase = (int)(GetTime()*flashSpeed) % 2;
            drawColor = (flashPhase == 0) ? RED : WHITE;
        }
        DrawTexturePro(texPlayer,
            {0,0,(float)texPlayer.width,(float)texPlayer.height},
            {playerPos.x,playerPos.y,
             texPlayer.width*pScale,
             texPlayer.height*pScale},
            {texPlayer.width*pScale/2,
             texPlayer.height*pScale/2},
            0, drawColor);
        
        for(auto &e : enemies)
        {
            float s = e.size/e.texture.width;
            DrawTexturePro(e.texture,
                {0,0,(float)e.texture.width,(float)e.texture.height},
                {e.pos.x,e.pos.y,
                 e.texture.width*s,
                 e.texture.height*s},
                {e.texture.width*s/2,
                 e.texture.height*s/2},
                0,WHITE);
        }
        
        for(auto &orb : xpOrbs)
        {
            float s = orb.size/orb.texture.width;
            DrawTexturePro(orb.texture,
                {0,0,(float)orb.texture.width,(float)orb.texture.height},
                {orb.pos.x,orb.pos.y,
                 orb.texture.width*s,
                 orb.texture.height*s},
                {orb.texture.width*s/2,
                 orb.texture.height*s/2},
                0,WHITE);
        }
        
        if(playerHP < playerMaxHP)
        {
            float hpBarW = playerSize;
            float hpBarH = 8;
            Vector2 hpPos = { playerPos.x - hpBarW/2 - 2, playerPos.y - playerSize/2 - 10 + 2 };
            DrawRectangle(hpPos.x, hpPos.y, hpBarW, hpBarH, RED);
            DrawRectangle(hpPos.x, hpPos.y, hpBarW*(playerHP/playerMaxHP), hpBarH, GREEN);
            DrawRectangleLines(hpPos.x, hpPos.y, hpBarW, hpBarH, WHITE);
        }
        
        for(auto &text : levelUpTexts)
        {
            DrawTextEx(gameFont, "LEVEL UP!", {text.pos.x - 50, text.pos.y}, 24, 1, YELLOW);
            text.pos.y -= 30.0f * GetFrameTime();
            text.timer -= GetFrameTime();
        }
        levelUpTexts.erase(std::remove_if(levelUpTexts.begin(), levelUpTexts.end(),
            [](LevelUpText &t){ return t.timer <= 0; }),
            levelUpTexts.end());

        EndMode2D();
        
        float barW = screenWidth-50-menuWidth;
        float barH = 18;
        float barX = 25;
        float barY = 5;

        DrawRectangle(barX,barY,(int)barW,barH,BLACK);
        DrawRectangle(barX,barY,(int)(barW*(playerXP/xpToLevel)),barH,SKYBLUE);
        DrawRectangleLines(barX,barY,(int)barW,barH,WHITE);

        std::string xpText = FormatXP(playerXP,xpToLevel);
        Vector2 xpSize = MeasureTextEx(gameFont,xpText.c_str(),18,1);
        DrawTextOutlined(gameFont,xpText,
            {barX+barW/2-xpSize.x/2,barY+barH+2},
            18,1,WHITE);

        std::string lvlText = "Lvl " + std::to_string(playerLevel);
        Vector2 lvlSize = MeasureTextEx(gameFont,lvlText.c_str(),16,1);
        DrawTextOutlined(gameFont,lvlText,
            {barX+10,barY+(barH/2)-(lvlSize.y/2)},
            16,1,WHITE);

        std::string timeStr = FormatTime(gameTime);
        DrawTextOutlined(gameFont,timeStr,{25,40},36,2,WHITE);

        float skullSize = 28;
        DrawTexturePro(texSkull,
            {0,0,(float)texSkull.width,(float)texSkull.height},
            {25,90,skullSize,skullSize},
            {0,0},0,WHITE);

        DrawTextOutlined(gameFont,std::to_string(totalKills),
            {25+skullSize+10,92},
            28,1,WHITE);
        
        debugMenu.Draw(screenWidth,screenHeight,
                       playerSpeed,
                       playerSize,
                       enemySpeed,
                       enemySize,
                       enemySpawnInterval,
                       spawnOnClick,
                       tileSize,
                       texGrass,
                       texPlayer,
                       texEnemy,
                       texXP,
                       gameFont,
                       enemies,
                       playerLevel,
                       playerXP,
                       xpToLevel,
                       xpOrbValue,
                       xpOrbs,
                       &killAllEnemiesFlag,
                       playerHP,
                       playerMaxHP);

        if(playerLevel > previousLevel)
        {
            for(int i = previousLevel; i < playerLevel; i++)
            {
                LevelUpText t;
                t.pos = { playerPos.x, playerPos.y - playerSize - 20 };
                t.timer = levelUpDuration;
                levelUpTexts.push_back(t);
            }
        }

        if(playerLevel < previousLevel)
        {
            playerXP = 0;
        }
        
        if(killAllEnemiesFlag)
        {
            totalKills += enemies.size();

            for(auto &e : enemies)
            {
                XPOrb orb;
                orb.pos = e.pos;
                orb.size = enemySize * 0.7f;
                orb.texture = texXP;
                xpOrbs.push_back(orb);
            }

            enemies.clear();
            killAllEnemiesFlag = false;
        }

        EndDrawing();
    }

    return 0;
}
