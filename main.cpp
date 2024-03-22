#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int rows = 10; 
    const int cols = 10; 
    const int rectWidth = 50; 
    const int rectHeight = 50;
    const int padding = 2; 

    InitWindow(screenWidth, screenHeight, "Grid of Rectangles");

    SetTargetFPS(60);
    
    Color colors[rows][cols];
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            colors[i][j] = GRAY;
        }
    }

    bool firstClick = false; 
    bool secondClick = false; 

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        int totalWidth = cols * rectWidth + (cols - 1) * padding;
        int totalHeight = rows * rectHeight + (rows - 1) * padding;
        
        int startX = (screenWidth - totalWidth) / 2;
        int startY = (screenHeight - totalHeight) / 2;
        
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++) {
                int x = startX + col * (rectWidth + padding);
                int y = startY + row * (rectHeight + padding);
                Rectangle rect = { x, y, rectWidth, rectHeight };

                DrawRectangleRec(rect, colors[row][col]);
                

                if (CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
  
                    if (!firstClick) {
                        colors[row][col] = GREEN;
                        firstClick = true;
                    }
                    
                    else if (!secondClick) {
                        colors[row][col] = RED;
                        secondClick = true;
                    }
                }
            }
        }
        EndDrawing();
      
    }

    CloseWindow();

    return 0;
}
