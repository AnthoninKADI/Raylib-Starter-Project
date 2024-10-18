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
} Ball;

Ball ball = {1, 1}; // Position initiale de la balle
Cell start = {1, 1}; // Point de départ
Cell end = {8, 1};   // Point d'arrivée

// Représentation de la grille : 0 = Normal, 1 = Route, 2 = Texture1, 3 = Texture2
int grid[GRID_SIZE][GRID_SIZE] = {
    {0, 3, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 5},
    {0, 1, 0, 4, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 6, 0, 1, 0, 7, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0},
};

// Direction pour les mouvements (haut, bas, gauche, droite)
int directions[4][2] = {
    {-1, 0}, // haut
    {1, 0},  // bas
    {0, -1}, // gauche
    {0, 1}   // droite
};

// Variable de vitesse de la balle
float ballSpeed = 1.0f; // Vitesse de la balle
float moveTimer = 0.0f; // Timer pour contrôler le mouvement
float moveInterval = 0.1f; // Temps entre chaque mouvement (en secondes)

// Texture variables
Texture2D texture1; // Texture pour 2
Texture2D texture2; // Texture pour 3
Texture2D texture3; // Texture pour 4
Texture2D texture4; // Texture pour 5
Texture2D texture5; // Texture pour 6
Texture2D texture6; // Texture pour 7

void DrawGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
            } else if (grid[i][j] == 1) {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
            } else if (grid[i][j] == 2) {
                DrawTexture(texture1, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 2
            } else if (grid[i][j] == 3) {
                DrawTexture(texture2, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 3
            }else if (grid[i][j] == 4) {
                DrawTexture(texture3, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 3
            }else if (grid[i][j] == 5) {
                DrawTexture(texture4, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 3
            }else if (grid[i][j] == 6) {
                DrawTexture(texture5, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 3
            }else if (grid[i][j] == 7) {
                DrawTexture(texture6, j * CELL_SIZE, i * CELL_SIZE, WHITE); // Affiche la texture pour 3
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
    InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, "Grid with Ball");
    
    // Charger les textures
    texture1 = LoadTexture("resources/BlueShop1.png"); // 2
    texture2 = LoadTexture("resources/BlueHouse1.png"); // 3
    texture3 = LoadTexture("resources/RedShop1.png"); // 4
    texture4 = LoadTexture("resources/RedHouse1.png"); // 5
    texture5 = LoadTexture("resources/PurpleShop1.png"); // 6 
    texture6 = LoadTexture("resources/PurpleHouse1.png"); // 7

    SetTargetFPS(60);

    Path path = {0};

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            int gridX = mousePos.x / CELL_SIZE;
            int gridY = mousePos.y / CELL_SIZE;

            if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                if (grid[gridY][gridX] == 1 || grid[gridY][gridX] == 2 || grid[gridY][gridX] == 3) {
                    // Changer la destination uniquement si la balle a atteint la destination actuelle
                    if (path.length == 0 || (ball.x == end.x && ball.y == end.y)) {
                        start.x = ball.x;  // Mettre à jour le point de départ à la position actuelle de la balle
                        start.y = ball.y;
                    }
                    end.x = gridX;
                    end.y = gridY;
                    path = AStar(start, end);
                }
            }
        }

        // Avancer la balle sur le chemin en fonction du timer
        if (path.length > 0) {
            moveTimer += GetFrameTime(); // Ajouter le temps écoulé à moveTimer

            if (moveTimer >= moveInterval) { // Vérifier si le temps écoulé dépasse l'intervalle
                ball.x = path.cells[0].x;
                ball.y = path.cells[0].y;
                for (int i = 0; i < path.length - 1; i++) {
                    path.cells[i] = path.cells[i + 1];
                }
                path.length--;
                moveTimer = 0; // Réinitialiser le timer
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGrid();
        // Dessiner la balle comme un cercle
        DrawCircle(ball.x * CELL_SIZE + CELL_SIZE / 2, ball.y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 4, RED); // Utiliser DrawCircle
        EndDrawing();
    }

    // Décharger les textures
    UnloadTexture(texture1);
    UnloadTexture(texture2);
    
    CloseWindow();
    return 0;
}
