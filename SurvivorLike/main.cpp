#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <cmath>

struct Player {
    Vector2 position;
    float radius;
    float speed;
};

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    const float debugWidth = 300.0f;

    InitWindow(screenWidth, screenHeight, "Vampire Survivor Like");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    Player player;
    player.position = { (screenWidth + debugWidth) / 2.0f, screenHeight / 2.0f };
    player.radius = 20.0f;
    player.speed = 300.0f;

    Color grassColor = { 144, 238, 144, 255 };

    while (!WindowShouldClose()) {

        float delta = GetFrameTime();

        // -------- Movement --------
        Vector2 direction = { 0.0f, 0.0f };

        if (IsKeyDown(KEY_W)) direction.y -= 1.0f;
        if (IsKeyDown(KEY_S)) direction.y += 1.0f;
        if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
        if (IsKeyDown(KEY_D)) direction.x += 1.0f;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.0f) {
            direction.x /= length;
            direction.y /= length;
        }

        player.position.x += direction.x * player.speed * delta;
        player.position.y += direction.y * player.speed * delta;

        // -------- DRAW --------
        BeginDrawing();

        // Zone jeu uniquement (à droite du menu)
        ClearBackground(DARKGRAY);

        // Dessin de la map (zone jeu)
        DrawRectangle(debugWidth, 0, screenWidth - debugWidth, screenHeight, grassColor);

        DrawCircleV(player.position, player.radius, WHITE);

        // -------- IMGUI --------
        rlImGuiBegin();

        // --- Style Violet ---
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 violet = ImVec4(0.4f, 0.0f, 0.6f, 1.0f);
        ImVec4 violetLight = ImVec4(0.6f, 0.2f, 0.8f, 1.0f);

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.0f, 0.18f, 1.0f);
        style.Colors[ImGuiCol_Header] = violet;
        style.Colors[ImGuiCol_HeaderHovered] = violetLight;
        style.Colors[ImGuiCol_Button] = violet;
        style.Colors[ImGuiCol_ButtonHovered] = violetLight;
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.0f, 0.35f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = violet;
        style.Colors[ImGuiCol_TitleBgActive] = violetLight;

        // -------- MENU GAUCHE FIXE --------
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(debugWidth, screenHeight));
        ImGui::Begin("Debug Menu",
                     nullptr,
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse);

        if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Size", &player.radius, 5.0f, 100.0f);
            ImGui::SliderFloat("Speed", &player.speed, 50.0f, 800.0f);
        }

        if (ImGui::CollapsingHeader("Map")) {
            ImGui::Text("Map settings coming soon...");
        }

        if (ImGui::CollapsingHeader("Enemy")) {
            ImGui::Text("Enemy settings coming soon...");
        }

        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
