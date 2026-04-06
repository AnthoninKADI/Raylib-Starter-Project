#include "UI.h"
#include "DoomLike.h"
#include "Player.h"

void DrawCrosshair(int size, int thickness)
{
    int cx = GetScreenWidth()/2;
    int cy = GetScreenHeight()/2;
    DrawRectangle(cx-size, cy-thickness/2, size*2, thickness, WHITE);
    DrawRectangle(cx-thickness/2, cy-size, thickness, size*2, WHITE);
}

void DrawMinimap(const Game& game)
{
    const int mapSize = 200;
    const int margin=20;
    const float scale = mapSize/(Level::W*Level::CELL);

    int startX = GetScreenWidth() - mapSize - margin;
    int startY = margin;

    DrawRectangle(startX-4,startY-4,mapSize+8,mapSize+8,BLACK);
    DrawRectangle(startX,startY,mapSize,mapSize,DARKGRAY);

    for(int z=0; z<Level::H; z++)
        for(int x=0; x<Level::W; x++)
            if(game.level.grid[z][x]==1)
            {
                int rx = startX + x*Level::CELL*scale;
                int ry = startY + z*Level::CELL*scale;
                int cellSize = Level::CELL*scale;
                DrawRectangle(rx,ry,cellSize,cellSize,GRAY);
            }

    float px = startX + game.player.pos.x*scale;
    float pz = startY + game.player.pos.z*scale;
    float angle = game.player.yaw;
    float size = 10.0f;
    Vector2 tip = { px + sinf(angle)*size, pz + cosf(angle)*size };
    Vector2 left= { px + sinf(angle+2.5f)*size*0.6f, pz + cosf(angle+2.5f)*size*0.6f };
    Vector2 right={ px + sinf(angle-2.5f)*size*0.6f, pz + cosf(angle-2.5f)*size*0.6f };
    DrawTriangle(tip,left,right,BLUE);

    for(auto& t: game.turrets) if(t.IsAlive()){ int tx=startX+t.pos.x*scale; int tz=startY+t.pos.z*scale; DrawCircle(tx,tz,6,RED); }
    for(auto& pack: game.healthPacks) if(pack.active){ int hx=startX+pack.pos.x*scale; int hz=startY+pack.pos.z*scale; DrawCircle(hx,hz,5,GREEN); }
    for(auto& pack: game.ammoPacks) if(pack.active){ int hx=startX+pack.pos.x*scale; int hz=startY+pack.pos.z*scale; DrawCircle(hx,hz,5,YELLOW); }
}

void DrawDamageFlash(const Player& player)
{
    if(player.damageFlash <= 0) return;
    float alpha = player.damageFlash / 0.4f;
    alpha = Clamp(alpha, 0.0f, 1.0f);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
        Color{255, 0, 0, (unsigned char)(150 * alpha)});
}

void DrawPlayerCoordinates(Vector3 playerPos)
{
    char coordsText[50];
    snprintf(coordsText, sizeof(coordsText), "X: %.2f / Y: %.2f / Z: %.2f", playerPos.x, playerPos.y, playerPos.z);
    
    int textX = GetScreenWidth() - 210; 
    int textY = 220;  
    int fontSize = 15;  
    
    DrawText(coordsText, textX, textY, fontSize, WHITE);
}