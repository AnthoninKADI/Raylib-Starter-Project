#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define GRID_SIZE 10
#define CELL_SIZE 50
#define MAX_PATH_LENGTH 100

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    Cell cells[MAX_PATH_LENGTH];
    int length;
} Path;

typedef struct {
    int x;
    int y;
    Path path;  
} Ball;

Ball ball1 = {1, 1}; // Blue ball Start
Cell end1 = {2, 8};  // Blue ball Finish

Ball ball2 = {8, 2}; // Red ball Start
Cell end2 = {4, 3};  // Red ball Finish

int grid[GRID_SIZE][GRID_SIZE] = {
    {0, 3, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 5},
    {0, 1, 0, 4, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0},
};

int directions[4][2] = {
    {-1, 0}, 
    {1, 0},  
    {0, -1}, 
    {0, 1}   
};

// Balls
float ballSpeed = 1.0f; 
float moveTimer1 = 0.0f; 
float moveTimer2 = 0.0f; 
float moveInterval = 0.1f; 

Texture2D texture1; // 2
Texture2D texture2; // 3
Texture2D texture3; // 4
Texture2D texture4; // 5

void DrawGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
            } else if (grid[i][j] == 1) {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
            } else if (grid[i][j] == 2) {
                DrawTexture(texture1, j * CELL_SIZE, i * CELL_SIZE, WHITE); 
            } else if (grid[i][j] == 3) {
                DrawTexture(texture2, j * CELL_SIZE, i * CELL_SIZE, WHITE); 
            } else if (grid[i][j] == 4) {
                DrawTexture(texture3, j * CELL_SIZE, i * CELL_SIZE, WHITE); 
            } else if (grid[i][j] == 5) {
                DrawTexture(texture4, j * CELL_SIZE, i * CELL_SIZE, WHITE); 
            }
        }
    }
}

int IsValid(int x, int y) {
    return (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && (grid[y][x] == 1 || grid[y][x] == 2 || grid[y][x] == 3));
}

Path AStar(Cell start, Cell end) {
    Path path = {0};
    Cell cameFrom[GRID_SIZE][GRID_SIZE] = {0};
    int gScore[GRID_SIZE][GRID_SIZE] = {0};
    int fScore[GRID_SIZE][GRID_SIZE] = {0};

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            gScore[i][j] = INT_MAX;
            fScore[i][j] = INT_MAX;
        }
    }

    gScore[start.y][start.x] = 0;
    fScore[start.y][start.x] = abs(start.x - end.x) + abs(start.y - end.y);

    Cell openSet[GRID_SIZE * GRID_SIZE];
    int openSetCount = 0;
    openSet[openSetCount++] = start;

    while (openSetCount > 0) {
        int currentIndex = 0;
        for (int i = 1; i < openSetCount; i++) {
            if (fScore[openSet[i].y][openSet[i].x] < fScore[openSet[currentIndex].y][openSet[currentIndex].x]) {
                currentIndex = i;
            }
        }

        Cell current = openSet[currentIndex];

        if (current.x == end.x && current.y == end.y) {
            Cell temp = current;
            while (1) {
                path.cells[path.length++] = temp;
                if (temp.x == start.x && temp.y == start.y) break;
                temp = cameFrom[temp.y][temp.x];
            }
            for (int i = 0; i < path.length / 2; i++) {
                Cell swap = path.cells[i];
                path.cells[i] = path.cells[path.length - 1 - i];
                path.cells[path.length - 1 - i] = swap;
            }
            return path;
        }

        for (int i = currentIndex; i < openSetCount - 1; i++) {
            openSet[i] = openSet[i + 1];
        }
        openSetCount--;

        for (int i = 0; i < 4; i++) {
            int neighborX = current.x + directions[i][1];
            int neighborY = current.y + directions[i][0];
            if (IsValid(neighborX, neighborY)) {
                Cell neighbor = {neighborX, neighborY};

                int tentative_gScore = gScore[current.y][current.x] + 1;

                if (tentative_gScore < gScore[neighbor.y][neighbor.x]) {
                    cameFrom[neighbor.y][neighbor.x] = current;
                    gScore[neighbor.y][neighbor.x] = tentative_gScore;
                    fScore[neighbor.y][neighbor.x] = gScore[neighbor.y][neighbor.x] + abs(neighbor.x - end.x) + abs(neighbor.y - end.y);

                    int alreadyInOpenSet = 0;
                    for (int j = 0; j < openSetCount; j++) {
                        if (openSet[j].x == neighbor.x && openSet[j].y == neighbor.y) {
                            alreadyInOpenSet = 1;
                            break;
                        }
                    }
                    if (!alreadyInOpenSet) {
                        openSet[openSetCount++] = neighbor;
                    }
                }
            }
        }
    }

    return path;
}

int main(void) {
    InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, "City Sim");
    
    // Charger les textures
    texture1 = LoadTexture("resources/BlueShop1.png"); // 2
    texture2 = LoadTexture("resources/BlueHouse1.png"); // 3
    texture3 = LoadTexture("resources/RedShop1.png"); // 4
    texture4 = LoadTexture("resources/RedHouse1.png"); // 5

    SetTargetFPS(60);
    
    Cell start1 = {ball1.x, ball1.y}; 
    Cell start2 = {ball2.x, ball2.y}; 
    
    ball1.path = AStar(start1, end1);
    ball2.path = AStar(start2, end2);
    
    while (!WindowShouldClose()) {
        // Ball Blue
        if (ball1.path.length > 0) {
            moveTimer1 += GetFrameTime(); 

            if (moveTimer1 >= moveInterval) { 
                ball1.x = ball1.path.cells[0].x;
                ball1.y = ball1.path.cells[0].y;
                for (int i = 0; i < ball1.path.length - 1; i++) {
                    ball1.path.cells[i] = ball1.path.cells[i + 1];
                }
                ball1.path.length--;
                moveTimer1 = 0; // Réinitialiser le timer
            }
        }

        // Ball Red
        if (ball2.path.length > 0) {
            moveTimer2 += GetFrameTime(); 

            if (moveTimer2 >= moveInterval) { 
                ball2.x = ball2.path.cells[0].x;
                ball2.y = ball2.path.cells[0].y;
                for (int i = 0; i < ball2.path.length - 1; i++) {
                    ball2.path.cells[i] = ball2.path.cells[i + 1];
                }
                ball2.path.length--;
                moveTimer2 = 0; 
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGrid();
        // Blue Ball
        DrawCircle(ball1.x * CELL_SIZE + CELL_SIZE / 2, ball1.y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 4, BLUE); 
        // Red Ball
        DrawCircle(ball2.x * CELL_SIZE + CELL_SIZE / 2, ball2.y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 4, RED); 
        EndDrawing();
    }
    
    UnloadTexture(texture1);
    UnloadTexture(texture2);
    UnloadTexture(texture3);
    UnloadTexture(texture4);
    
    CloseWindow();
    return 0;
}
