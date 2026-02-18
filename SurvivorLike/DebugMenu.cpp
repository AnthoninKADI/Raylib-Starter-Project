#include "DebugMenu.h"
#include "rlImGui.h"
#include "imgui.h"
#include <algorithm>

DebugMenu::DebugMenu(float width)
{
    menuWidth = width;

    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 violet      = ImVec4(0.45f, 0.0f, 0.55f, 1.0f);
    ImVec4 violetDark  = ImVec4(0.30f, 0.0f, 0.40f, 1.0f);
    ImVec4 pinkSoft    = ImVec4(0.85f, 0.2f, 0.6f, 1.0f);
    ImVec4 frameDark   = ImVec4(0.15f, 0.0f, 0.20f, 1.0f);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.0f, 0.12f, 1.0f);

    style.Colors[ImGuiCol_TitleBg]          = pinkSoft;
    style.Colors[ImGuiCol_TitleBgActive]    = pinkSoft;
    style.Colors[ImGuiCol_TitleBgCollapsed] = violetDark;

    style.Colors[ImGuiCol_Header]        = violet;
    style.Colors[ImGuiCol_HeaderHovered] = pinkSoft;
    style.Colors[ImGuiCol_HeaderActive]  = violetDark;

    style.Colors[ImGuiCol_Button]        = violet;
    style.Colors[ImGuiCol_ButtonHovered] = pinkSoft;
    style.Colors[ImGuiCol_ButtonActive]  = violetDark;

    style.Colors[ImGuiCol_FrameBg]        = frameDark;
    style.Colors[ImGuiCol_FrameBgHovered] = pinkSoft;
    style.Colors[ImGuiCol_FrameBgActive]  = violetDark;

    style.Colors[ImGuiCol_SliderGrab]       = violet;
    style.Colors[ImGuiCol_SliderGrabActive] = pinkSoft;

    style.FrameRounding  = 6.0f;
    style.GrabRounding   = 6.0f;
    style.WindowRounding = 10.0f;
}

void DebugMenu::Draw(float screenWidth, float screenHeight,
                     float& playerSpeed,
                     float& playerSize,
                     float& enemySpeed,
                     float& enemySize,
                     float& enemySpawnInterval,
                     bool& spawnOnClick,
                     float& tileSize,
                     Texture2D& texGrass,
                     Texture2D& texPlayer,
                     Texture2D& texEnemy,
                     Texture2D& texXP,
                     Font& gameFont,
                     std::vector<Enemy>& enemies,
                     int& playerLevel,
                     float& playerXP,
                     float& xpToLevel,
                     float& xpOrbValue,
                     std::vector<XPOrb>& xpOrbs,
                     bool* killAllEnemiesFlag,
                     float& playerHP,
                     float& playerMaxHP)
{
    rlImGuiBegin();

    ImGui::SetNextWindowPos(ImVec2(screenWidth - menuWidth, 0));
    ImGui::SetNextWindowSize(ImVec2(menuWidth, screenHeight));

    ImGui::Begin("Debug Menu", nullptr,
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Speed", &playerSpeed, 50.0f, 1000.0f);
        ImGui::SliderFloat("Size", &playerSize, 20.0f, 200.0f);

        ImGui::Separator();
        ImGui::Text("Level Controls");

        if(ImGui::Button("+1"))  playerLevel += 1;
        ImGui::SameLine();
        if(ImGui::Button("+5"))  playerLevel += 5;
        ImGui::SameLine();
        if(ImGui::Button("+10")) playerLevel += 10;

        if(ImGui::Button("-1"))  playerLevel = std::max(1, playerLevel - 1);
        ImGui::SameLine();
        if(ImGui::Button("-5"))  playerLevel = std::max(1, playerLevel - 5);
        ImGui::SameLine();
        if(ImGui::Button("-10")) playerLevel = std::max(1, playerLevel - 10);

        ImGui::SliderFloat("XP Orb Value", &xpOrbValue, 1.0f, 100.0f);

        ImGui::Separator();
        ImGui::Text("HP Controls");
        ImGui::SliderFloat("HP", &playerHP, 0.0f, playerMaxHP);
        ImGui::SliderFloat("Max HP", &playerMaxHP, 1.0f, 500.0f);
        if(ImGui::Button("Damage 10"))  playerHP = std::max(0.0f, playerHP - 10.0f);
        ImGui::SameLine();
        if(ImGui::Button("Heal 10"))    playerHP = std::min(playerMaxHP, playerHP + 10.0f);
    }

    if (ImGui::CollapsingHeader("Enemy", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Enemy Speed", &enemySpeed, 10.0f, 500.0f);
        ImGui::SliderFloat("Enemy Size", &enemySize, 20.0f, 200.0f);
        ImGui::SliderFloat("Spawn Interval", &enemySpawnInterval, 0.1f, 10.0f);

        ImGui::Checkbox("Spawn On Click", &spawnOnClick);

        ImGui::Separator();

        if(ImGui::Button("Kill Enemies", ImVec2(-1,40)))
        {
            if(killAllEnemiesFlag)
                *killAllEnemiesFlag = true;
        }
    }

    if (ImGui::CollapsingHeader("Map", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Tile Size", &tileSize, 16.0f, 256.0f);
    }

    ImGui::End();
    rlImGuiEnd();
}
