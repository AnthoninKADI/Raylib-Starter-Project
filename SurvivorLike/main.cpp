#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

int main(){
	const int w = 700;
	const int h = 700;
	InitWindow(w, h, "Raylib Window");
	SetTargetFPS(60);

	rlImGuiSetup(true);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		rlImGuiBegin();

		ImGui::Begin("Hello", NULL);
		ImGui::Text("Hello from ImGui");
		ImGui::End();

		rlImGuiEnd();

		EndDrawing();
	}
	rlImGuiShutdown();
	CloseWindow();
	return 0;
}