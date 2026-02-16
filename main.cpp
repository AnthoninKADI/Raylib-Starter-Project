#include "raylib.h"
#include "raymath.h"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Survivor Prototype");
    SetTargetFPS(144);


    Texture2D playerTexture = LoadTexture("assets/textures/PlayerSlime.png");
    Texture2D grassTexture  = LoadTexture("assets/textures/Grass.png");


    SetTextureFilter(playerTexture, TEXTURE_FILTER_POINT);
    SetTextureFilter(grassTexture, TEXTURE_FILTER_POINT);


    Vector2 playerPos = { 0.0f, 0.0f };
    float playerSpeed = 300.0f;
    float playerScale = 2.5f;


    float grassScale = 0.2f;


    Camera2D camera = { 0 };
    camera.target = playerPos;
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();


        Vector2 direction = { 0.0f, 0.0f };

        if (IsKeyDown(KEY_W)) direction.y -= 1.0f;
        if (IsKeyDown(KEY_S)) direction.y += 1.0f;
        if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
        if (IsKeyDown(KEY_D)) direction.x += 1.0f;


        if (direction.x != 0.0f || direction.y != 0.0f)
        {
            direction = Vector2Normalize(direction);
        }


        playerPos.x += direction.x * playerSpeed * dt;
        playerPos.y += direction.y * playerSpeed * dt;


        camera.target = playerPos;

 
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        
        Rectangle grassSource = { 0, 0, (float)grassTexture.width, (float)grassTexture.height };

        float tileSize = grassTexture.width * grassScale;

        for (int x = -40; x < 40; x++)
        {
            for (int y = -40; y < 40; y++)
            {
                Rectangle grassDest = {
                    x * tileSize,
                    y * tileSize,
                    tileSize,
                    tileSize
                };

                DrawTexturePro(grassTexture, grassSource, grassDest, { 0, 0 }, 0.0f, WHITE);
            }
        }
        Rectangle playerSource = { 0, 0, (float)playerTexture.width, (float)playerTexture.height };

        Rectangle playerDest = {
            playerPos.x,
            playerPos.y,
            playerTexture.width * playerScale,
            playerTexture.height * playerScale
        };

        Vector2 playerOrigin = {
            playerDest.width / 2,
            playerDest.height / 2
        };

        DrawTexturePro(playerTexture, playerSource, playerDest, playerOrigin, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadTexture(playerTexture);
    UnloadTexture(grassTexture);

    CloseWindow();

    return 0;
}
