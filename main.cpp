#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int rows =10; // Number of rows in the grid
    const int cols = 10; // Number of columns in the grid
    const int rectWidth = 50; // Width of each rectangle
    const int rectHeight = 50; // Height of each rectangle
    const int padding = 2; // Padding between rectangles

    InitWindow(screenWidth, screenHeight, "Grid of Rectangles");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Calculate the total width and height of the grid
        int totalWidth = cols * rectWidth + (cols - 1) * padding;
        int totalHeight = rows * rectHeight + (rows - 1) * padding;

        // Calculate the starting position to center the grid
        int startX = (screenWidth - totalWidth) / 2;
        int startY = (screenHeight - totalHeight) / 2;

        // Draw the grid of rectangles
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                int x = startX + col * (rectWidth + padding);
                int y = startY + row * (rectHeight + padding);
                Rectangle rect = { x, y, rectWidth, rectHeight };
                // Change the color to black and white (GRAY)
                DrawRectangleRec(rect, GRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}