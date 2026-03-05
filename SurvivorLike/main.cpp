#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "DebugMenu.h"
#include "PauseMenu.h"
#include "MainMenu.h"
#include "Entities.h"
#include <vector>
#include <cmath>
#include <string>
#include <random>
#include <algorithm>

#include "UpgradeMenu.h"

const int screenWidth  = 1600;
const int screenHeight = 900;
const float menuWidth  = 350.0f;

float tileSize = 80.0f;
const int mapWidth  = 200;
const int mapHeight = 200;

Vector2 playerPos;
PlayerStats playerStats;

UpgradeMenu upgradeMenu(screenWidth, screenHeight);
bool levelJustIncreased = false; 

float playerSpeed = 400.0f;
float playerSize  = 120.0f;
int playerLevel   = 1;
float playerXP    = 0;
float xpToLevel   = 20.0f + pow(playerLevel,2.0f)*12.0f;
float xpOrbValue  = 10.0f;

float playerMaxHP = 100.0f;
float playerHP    = 100.0f;
float playerInvincibilityTimer = 0.0f;
bool levelUpPending = false;
void UpgradeSpeed(PlayerStats& player) { playerSpeed += 20; }
void UpgradeDamage(PlayerStats& player) { player.damage += 5; }
void UpgradeHP(PlayerStats& player) { playerMaxHP += 20; }

int totalKills = 0;
float levelUpDuration = 2.0f;

float Distance(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

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
float enemySpawnRadius   = 300.0f;
bool showSpawnRadius     = false;

std::vector<XPOrb> xpOrbs;

std::vector<Projectile> projectiles;
float projectileSpeed = 400.0f;
float projectileCooldown = 1.5f;
float projectileTimer = 0.0f;

struct Tile { Vector2 pos; Texture2D texture; };
std::vector<Tile> mapTiles;

float gameTime = 0.0f;
bool killAllEnemiesFlag = false;

std::vector<UpgradeOption> allUpgrades =
{
    {
        "Damage Up",
        "+5 damage",
        UpgradeDamage
    },

    {
        "Move Speed",
        "+20 movement speed",
        UpgradeSpeed
    },

    {
        "Max HP",
        "+20 max health",
        UpgradeHP
    },

    {
        "Attack Speed",
        "Shoot faster",
        [](PlayerStats& player)
        {
            projectileCooldown *= 0.50f;
        }
    },

    {
        "Projectile Speed",
        "Faster bullets",
        [](PlayerStats& player)
        {
            projectileSpeed += 20.0f;
        }
    },

    {
        "Heal",
        "Recover 30 HP",
        [](PlayerStats& player)
        {
            playerHP += 20;
            if (playerHP > playerMaxHP)
                playerHP = playerMaxHP;
        }
    }
};

std::vector<UpgradeOption> GetRandomUpgrades(int count)
{
    std::vector<UpgradeOption> pool = allUpgrades;

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(pool.begin(), pool.end(), g);

    if (pool.size() > count)
        pool.resize(count);

    return pool;
}

void DrawSettingsMenu(float screenWidth, float screenHeight, bool &showSettings, bool &fullscreen, float &mouseSensitivity, MainMenu::AimMode &aimMode);

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

void ResetGame()
{
    playerPos = { mapWidth*tileSize/2.0f, mapHeight*tileSize/2.0f };
    playerHP = playerMaxHP;
    playerLevel = 1;
    playerXP = 0;
    xpToLevel = 20.0f + pow(playerLevel,2.0f)*12.0f;
    totalKills = 0;

    enemies.clear();
    xpOrbs.clear();
    projectiles.clear();

    enemySpawnTimer = 0.0f;      
    projectileTimer = 0.0f;      
    gameTime = 0.0f;             
    levelUpTexts.clear();        

    playerStats.moveSpeed          = playerSpeed;
    playerStats.lifeSteal          = 0.0f;
    playerStats.damage             = 1.0f;
    playerStats.projectileCooldown = projectileCooldown;
    playerStats.maxHP              = playerMaxHP;
    playerStats.projectileCount    = 1;
    playerStats.effectDuration     = 0.0f;
    playerStats.projectilePierce   = 1; 
}


void ForceLevelChange(int delta)
{
    int oldLevel = playerLevel;
    playerLevel += delta;
    if(playerLevel < 1) playerLevel = 1;
    xpToLevel = 20.0f + pow(playerLevel, 2.0f) * 12.0f;

    if(playerLevel > oldLevel)
    {
        levelUpPending = true;

        LevelUpText t;
        t.pos = { playerPos.x, playerPos.y - playerSize - 20 };
        t.timer = levelUpDuration;
        levelUpTexts.push_back(t);
    }
}

int main()
{
    InitWindow(screenWidth, screenHeight,"Vampire Survivor Base");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    rlImGuiSetup(true);
    
    MainMenu mainMenu((float)screenWidth, (float)screenHeight);
    PauseMenu pauseMenu((float)screenWidth, (float)screenHeight);
    bool inGame = false;
    bool paused = false;
    bool showSettings = false;
    bool fullscreen = false;
    float mouseSensitivity = 1.0f;
    MainMenu::AimMode currentAimMode = MainMenu::AimMode::ClosestEnemy;

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
    const float passiveRegen = 1.0f / 60.0f;
    const float flashSpeed = 15.0f;

    while(!WindowShouldClose())
    {
        if(!inGame)
        {
            mainMenu.Update();

            if(mainMenu.ShouldStartGame())
            {
                inGame = true;
                mainMenu.ResetFlags();
                ResetGame();

                MainMenu::Stats selStats = mainMenu.GetSelectedStats();
                playerStats.moveSpeed        = selStats.speed;
                playerStats.lifeSteal        = selStats.lifesteal;
                playerStats.damage           = selStats.damage;
                playerStats.projectileCooldown = selStats.attackCooldown;
                playerStats.maxHP            = selStats.maxHP;
                playerStats.projectileCount  = selStats.shots;
                playerStats.effectDuration   = selStats.effectDuration;
                playerStats.projectilePierce = selStats.enemiesPierced;

                playerSpeed   = playerStats.moveSpeed;
                playerMaxHP   = playerStats.maxHP;
                playerHP      = playerStats.maxHP;
                projectileCooldown = playerStats.projectileCooldown;
                texPlayer     = mainMenu.GetSelectedTexture();
                currentAimMode = mainMenu.GetAimMode();
            }

            if(mainMenu.ShouldQuit())
            {
                CloseWindow();
                return 0;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            mainMenu.Draw();
            EndDrawing();
            continue;
        }

        if(IsKeyPressed(KEY_ESCAPE))
            pauseMenu.Toggle();

        if(pauseMenu.IsPaused())
        {
            if(!showSettings)
            {
                pauseMenu.Update();

                if(pauseMenu.ResumeRequested())
                    pauseMenu.Toggle();

                if(pauseMenu.SettingsRequested())
                {
                    showSettings = true;
                    pauseMenu.ClearSettingsRequest();
                }

                if(pauseMenu.BackToMenuRequested())
                {
                    inGame = false;
                    pauseMenu.Toggle();
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);

            pauseMenu.Draw();

            if(showSettings)
                DrawSettingsMenu(screenWidth, screenHeight, showSettings, fullscreen, mouseSensitivity, currentAimMode);

            EndDrawing();
            continue;
        }

        if(upgradeMenu.IsActive())
        {
            upgradeMenu.Update(playerStats);
            upgradeMenu.Draw();
            EndDrawing();
            continue;
        }

        if(levelUpPending && !upgradeMenu.IsActive())
        {
            auto upgrades = GetRandomUpgrades(3);
            upgradeMenu.Show(upgrades);
            levelUpPending = false;
        }

        float delta = GetFrameTime();
        gameTime += delta;
        projectileTimer += delta;
        if(playerInvincibilityTimer > 0) playerInvincibilityTimer -= delta;
        else playerInvincibilityTimer = 0;

        if(playerHP < playerMaxHP && playerInvincibilityTimer <= 0)
            playerHP = std::min(playerMaxHP, playerHP + passiveRegen*delta*60.0f);

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

            if(!spawnOnClick) 
            {
                float angle = GetRandomValue(0,359) * DEG2RAD;
                Enemy e;
                e.size = enemySize;
                e.speed = enemySpeed;
                e.texture = texEnemy;
                e.pos = { playerPos.x + cos(angle)*enemySpawnRadius,
                          playerPos.y + sin(angle)*enemySpawnRadius };
                enemies.push_back(e);
            }
        }
        
        if(spawnOnClick && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Enemy e;
            e.size = enemySize;
            e.speed = enemySpeed;
            e.texture = texEnemy;
            e.pos = GetScreenToWorld2D(GetMousePosition(), camera);
            enemies.push_back(e);
        }


        for(auto &e : enemies)
        {
            Vector2 d = { playerPos.x - e.pos.x, playerPos.y - e.pos.y };
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

        for(int i = 0; i < xpOrbs.size(); )
        {
            float dx = playerPos.x - xpOrbs[i].pos.x;
            float dy = playerPos.y - xpOrbs[i].pos.y;
            float dist = sqrt(dx*dx + dy*dy);
            float pickupRadius = playerSize * 0.6f;

            if(dist < pickupRadius)
            {
                playerXP += xpOrbValue;
                xpOrbs.erase(xpOrbs.begin() + i);
            }
            else i++;
        }

        while(playerXP >= xpToLevel)
        {
            playerXP -= xpToLevel;
            ForceLevelChange(1);
        }

        if(projectileTimer >= projectileCooldown)
        {
            projectileTimer = 0.0f;

            Vector2 shootDir = {0,0};
            bool canShoot = false;

            if(currentAimMode == MainMenu::AimMode::ClosestEnemy)
            {
                if(!enemies.empty())
                {
                    Enemy* closest = nullptr;
                    float minDist = 999999.0f;
                    for(auto &e : enemies)
                    {
                        float d = Distance(playerPos,e.pos);
                        if(d < minDist)
                        {
                            minDist = d;
                            closest = &e;
                        }
                    }
                    if(closest)
                    {
                        shootDir = {closest->pos.x - playerPos.x, closest->pos.y - playerPos.y};
                        canShoot = true;
                    }
                }
            }
            else if(currentAimMode == MainMenu::AimMode::MousePosition)
            {
                Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
                shootDir = {mouseWorld.x - playerPos.x, mouseWorld.y - playerPos.y};
                canShoot = true;
            }

            if(canShoot)
            {
                float len = sqrt(shootDir.x*shootDir.x + shootDir.y*shootDir.y);
                if(len>0)
                {
                    shootDir.x/=len;
                    shootDir.y/=len;

                    Projectile p;
                    p.pos = playerPos;
                    p.dir = shootDir;
                    p.speed = projectileSpeed;
                    p.active = true;
                    p.range = 1000.0f;
                    p.travelled = 0.0f;
                    p.pierceCount = 0;
                    projectiles.push_back(p);
                }
            }
        }

        for(int i = 0; i < projectiles.size(); i++)
        {
            if(!projectiles[i].active) continue;
            float dx = projectiles[i].dir.x * projectiles[i].speed * delta;
            float dy = projectiles[i].dir.y * projectiles[i].speed * delta;

            projectiles[i].pos.x += dx;
            projectiles[i].pos.y += dy;
            projectiles[i].travelled += sqrt(dx*dx + dy*dy);
            if(projectiles[i].travelled >= projectiles[i].range)
                projectiles[i].active = false;

            for(int j = 0; j < enemies.size(); j++)
            {
                float d = Distance(projectiles[i].pos,enemies[j].pos);
                if(d < (enemies[j].size/2))
                {
                    XPOrb orb;
                    orb.pos = enemies[j].pos;
                    orb.size = enemies[j].size*0.7f;
                    orb.value = 1;
                    orb.texture = texXP;
                    xpOrbs.push_back(orb);

                    totalKills++;

                    enemies.erase(enemies.begin()+j);

                    projectiles[i].pierceCount++;
                    if(projectiles[i].pierceCount >= playerStats.projectilePierce)
                        projectiles[i].active = false;

                    break;
                }
            }
        }
        projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),
                                         [](Projectile &p){ return !p.active; }),
                          projectiles.end());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        for(auto &tile : mapTiles)
        {
            DrawTexturePro(tile.texture,
                {0,0,(float)tile.texture.width,(float)tile.texture.height},
                {tile.pos.x,tile.pos.y,tileSize,tileSize},
                {0,0},0,WHITE);
        }

        float pScale = playerSize/texPlayer.width;
        Color drawColor = WHITE;
        if(playerInvincibilityTimer > 0)
        {
            int flashPhase = (int)(GetTime()*15.0f) % 2;
            drawColor = (flashPhase == 0) ? RED : WHITE;
        }
        DrawTexturePro(texPlayer,
            {0,0,(float)texPlayer.width,(float)texPlayer.height},
            {playerPos.x,playerPos.y,texPlayer.width*pScale,texPlayer.height*pScale},
            {texPlayer.width*pScale/2,texPlayer.height*pScale/2},
            0, drawColor);

        if(playerHP < playerMaxHP)
        {
            float hpBarW = 80;
            float hpBarH = 8;
            Vector2 hpPos = { playerPos.x - hpBarW/2 - 2, playerPos.y - playerSize/2 - 10 + 2 };
            DrawRectangle(hpPos.x, hpPos.y, hpBarW, hpBarH, RED);
            DrawRectangle(hpPos.x, hpPos.y, hpBarW*(playerHP/playerMaxHP), hpBarH, GREEN);
            DrawRectangleLines(hpPos.x, hpPos.y, hpBarW, hpBarH, WHITE);
        }

        for(auto &text : levelUpTexts)
        {
            DrawTextEx(gameFont, "LEVEL UP!", {text.pos.x - 50, text.pos.y}, 24, 1, YELLOW);
            text.pos.y -= 30.0f * delta;
            text.timer -= delta;
        }
        levelUpTexts.erase(std::remove_if(levelUpTexts.begin(), levelUpTexts.end(),
                                          [](LevelUpText &t){ return t.timer <= 0; }),
                           levelUpTexts.end());

        for(auto &e : enemies)
        {
            float s = e.size/e.texture.width;
            DrawTexturePro(e.texture,
                {0,0,(float)e.texture.width,(float)e.texture.height},
                {e.pos.x,e.pos.y,e.texture.width*s,e.texture.height*s},
                {e.texture.width*s/2,e.texture.height*s/2},
                0,WHITE);
        }

        for(auto &orb : xpOrbs)
        {
            float s = orb.size/orb.texture.width;
            DrawTexturePro(orb.texture,
                {0,0,(float)orb.texture.width,(float)orb.texture.height},
                {orb.pos.x,orb.pos.y,orb.texture.width*s,orb.texture.height*s},
                {orb.texture.width*s/2,orb.texture.height*s/2},
                0,WHITE);
        }

        for(auto &p : projectiles)
            DrawCircleV(p.pos,6,WHITE);

        if(showSpawnRadius)
            DrawCircleLines(playerPos.x,playerPos.y,enemySpawnRadius,RED);

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

        debugMenu.Draw(
            screenWidth, screenHeight,
            playerSpeed, playerSize,
            enemySpeed, enemySize,
            enemySpawnInterval, spawnOnClick,
            tileSize, texGrass, texPlayer, texEnemy, texXP,
            gameFont, enemies,
            playerLevel, playerXP, xpToLevel, xpOrbValue,
            xpOrbs, &killAllEnemiesFlag,
            playerHP, playerMaxHP,
            enemySpawnRadius, showSpawnRadius,
            projectiles, projectileSpeed, projectileCooldown,
            ResetGame, ForceLevelChange
        );

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
