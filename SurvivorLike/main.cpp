#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <cmath>
#include <vector>
#include <string>

enum GameState {
    MENU,
    SELECT,
    GAME
};

struct Player {
    Vector2 position;
    float radius;
    float speed;
    Color color;
};

struct Card {
    Rectangle rect;
    Color color;
    std::string title;
    std::string description;
};

int main() {

    const int screenWidth = 1600;
    const int screenHeight = 900;
    const float debugWidth = 300.0f;

    InitWindow(screenWidth, screenHeight, "Vampire Survivor Like");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    GameState state = MENU;

    Player player;
    player.radius = 20;
    player.speed = 300;
    player.position = { 1000, 450 };
    player.color = WHITE;

    Color grassColor = {144, 238, 144, 255};

    float cardWidth = 250;
    float cardHeight = 350;
    float spacing = 80;

    std::vector<Card> cards;

    float totalWidth = (cardWidth * 3) + (spacing * 2);
    float startX = (screenWidth - totalWidth) / 2.0f;
    float centerY = screenHeight / 2.0f - cardHeight / 2.0f;

    cards.push_back({
        {startX, centerY, cardWidth, cardHeight},
        RED,
        "Blood Reaper",
        "Enemies burn with rage.\nVery angry tomatoes."
    });

    cards.push_back({
        {startX + cardWidth + spacing, centerY, cardWidth, cardHeight},
        BLUE,
        "Frost Walker",
        "Enemies are cool.\nLiterally too cool."
    });

    cards.push_back({
        {startX + (cardWidth + spacing) * 2, centerY, cardWidth, cardHeight},
        PURPLE,
        "Void Spawn",
        "Enemies from the void.\nProbably unpaid interns."
    });

    int currentTheme = 0;

    while (!WindowShouldClose()) {

        float delta = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        if (state == GAME) {

            Vector2 direction = {0,0};

            if (IsKeyDown(KEY_W)) direction.y -= 1;
            if (IsKeyDown(KEY_S)) direction.y += 1;
            if (IsKeyDown(KEY_A)) direction.x -= 1;
            if (IsKeyDown(KEY_D)) direction.x += 1;

            float length = sqrt(direction.x*direction.x + direction.y*direction.y);
            if (length > 0) {
                direction.x /= length;
                direction.y /= length;
            }

            player.position.x += direction.x * player.speed * delta;
            player.position.y += direction.y * player.speed * delta;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (state == MENU) {

            DrawText("VAMPIRE SURVIVOR LIKE", screenWidth/2 - 250, 200, 40, WHITE);

            Rectangle playBtn = { screenWidth/2 - 100, 400, 200, 80 };
            bool hover = CheckCollisionPointRec(mouse, playBtn);

            DrawRectangleRec(playBtn, hover ? DARKPURPLE : PURPLE);
            DrawText("PLAY", playBtn.x + 60, playBtn.y + 25, 30, WHITE);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                state = SELECT;
        }

        else if (state == SELECT) {

            DrawText("Choose Your Player Color",
                     screenWidth/2 - 250,
                     120,
                     40,
                     WHITE);

            for (int i = 0; i < cards.size(); i++) {

                Card& card = cards[i];
                bool hover = CheckCollisionPointRec(mouse, card.rect);

                float scale = hover ? 1.05f : 1.0f;

                Rectangle scaled = {
                    card.rect.x - (card.rect.width*(scale-1)/2),
                    card.rect.y - (card.rect.height*(scale-1)/2),
                    card.rect.width * scale,
                    card.rect.height * scale
                };

                DrawRectangleRec(scaled, Color{40,40,40,255});

                if (hover)
                    DrawRectangleLinesEx(scaled, 4, card.color);

                DrawCircle(scaled.x + scaled.width/2,
                           scaled.y + 120,
                           40,
                           card.color);

                DrawText(card.title.c_str(),
                         scaled.x + 20,
                         scaled.y + 200,
                         22,
                         WHITE);

                DrawText(card.description.c_str(),
                         scaled.x + 20,
                         scaled.y + 240,
                         18,
                         GRAY);

                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentTheme = i;
                    player.color = cards[i].color;
                    state = GAME;
                }
            }
        }

        else if (state == GAME) {

            DrawRectangle(debugWidth, 0,
                          screenWidth - debugWidth,
                          screenHeight,
                          grassColor);

            DrawCircleV(player.position, player.radius, player.color);

            rlImGuiBegin();

            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(ImVec2(debugWidth, screenHeight));
            ImGui::Begin("Debug Menu", nullptr,
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse);

            if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderFloat("Size", &player.radius, 5, 100);
                ImGui::SliderFloat("Speed", &player.speed, 50, 800);

                if (ImGui::TreeNode("Change Player Theme"))
                {
                    for (int i = 0; i < cards.size(); i++)
                    {
                        ImGui::PushID(i);

                        bool isActive = (currentTheme == i);

                        ImVec4 btnColor = ImVec4(
                            cards[i].color.r / 255.0f,
                            cards[i].color.g / 255.0f,
                            cards[i].color.b / 255.0f,
                            1.0f
                        );

                        ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                              ImVec4(
                                                  fmin(btnColor.x + 0.2f, 1.0f),
                                                  fmin(btnColor.y + 0.2f, 1.0f),
                                                  fmin(btnColor.z + 0.2f, 1.0f),
                                                  1.0f));

                        if (ImGui::Button(cards[i].title.c_str(), ImVec2(200, 40)))
                        {
                            currentTheme = i;
                            player.color = cards[i].color;
                        }

                        if (isActive)
                            ImGui::Text("Current");

                        ImGui::PopStyleColor(2);
                        ImGui::PopID();
                    }

                    ImGui::TreePop();
                }
            }

            if (ImGui::CollapsingHeader("Map"))
                ImGui::Text("Soon...");

            if (ImGui::CollapsingHeader("Enemy"))
                ImGui::Text("Soon...");

            ImGui::End();
            rlImGuiEnd();
        }

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
