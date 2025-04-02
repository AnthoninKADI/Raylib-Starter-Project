#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>
#include <mutex>

#define GRID_WIDTH 15
#define GRID_HEIGHT 15
#define TILE_SIZE 40

struct PositionComponent {
    float x, y;
};

struct VelocityComponent {
    float dx, dy;
};

struct PlayerComponent {
    bool isPlayer;
};

struct EnemyComponent {
    bool isEnemy;
};

struct PillComponent {
    bool isPill;
};

enum Tile {
    WALL,
    PATH
};

std::vector<std::vector<Tile>> maze(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));
std::vector<PositionComponent> pillPositions;
std::vector<PositionComponent> enemyPositions;

std::mutex mutexPill;
std::mutex mutexEnemy;

void GenerateMaze(std::vector<std::vector<Tile>>& maze) {
    srand(time(NULL));
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            maze[y][x] = WALL;
        }
    }

    for (int y = 1; y < GRID_HEIGHT - 1; y += 2) {
        for (int x = 1; x < GRID_WIDTH - 1; x += 2) {
            maze[y][x] = PATH;
            if (x + 1 < GRID_WIDTH && maze[y][x + 1] == WALL) {
                maze[y][x + 1] = PATH;
            }
            if (y + 1 < GRID_HEIGHT && maze[y + 1][x] == WALL) {
                maze[y + 1][x] = PATH;
            }
        }
    }
}

bool IsPath(const std::vector<std::vector<Tile>>& maze, int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
    return maze[y][x] == PATH;
}

void MoveEnemyTowardsPlayer(PositionComponent& enemyPos, const PositionComponent& playerPos, const std::vector<std::vector<Tile>>& maze, float speed) {
    int enemyX = (int)(enemyPos.x / TILE_SIZE);
    int enemyY = (int)(enemyPos.y / TILE_SIZE);
    int playerX = (int)(playerPos.x / TILE_SIZE);
    int playerY = (int)(playerPos.y / TILE_SIZE);

    int dx = 0, dy = 0;
    if (playerX > enemyX) dx = 1;
    if (playerX < enemyX) dx = -1;
    if (playerY > enemyY) dy = 1;
    if (playerY < enemyY) dy = -1;

    if (IsPath(maze, enemyX + dx, enemyY)) {
        enemyPos.x += dx * speed;
    }

    if (IsPath(maze, enemyX, enemyY + dy)) {
        enemyPos.y += dy * speed;
    }
}

void ParallelMoveEnemies(std::vector<PositionComponent>& enemyPositions, const PositionComponent& playerPosition, const std::vector<std::vector<Tile>>& maze) {
    std::vector<std::thread> threads;
    for (auto& enemy : enemyPositions) {
        threads.push_back(std::thread(MoveEnemyTowardsPlayer, std::ref(enemy), playerPosition, std::ref(maze), 0.05f));
    }
    
    for (auto& th : threads) {
        th.join();
    }
}

bool CheckCollision(const PositionComponent& playerPos, const PositionComponent& entityPos, float radius) {
    float distX = playerPos.x - entityPos.x;
    float distY = playerPos.y - entityPos.y;
    float distance = sqrt(distX * distX + distY * distY);
    return distance < radius * 2;
}

void PlacePills(std::vector<std::vector<Tile>>& maze, std::vector<PositionComponent>& pillPositions) {
    for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GRID_WIDTH - 1; ++x) {
            if (maze[y][x] == PATH && GetRandomValue(0, 10) > 7) {
                pillPositions.push_back({(float)(x * TILE_SIZE + TILE_SIZE / 2), (float)(y * TILE_SIZE + TILE_SIZE / 2)});
            }
        }
    }
}

void DrawMaze(const std::vector<std::vector<Tile>>& maze) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (maze[y][x] == WALL) {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }
    }
}


void DrawEntities(const PositionComponent& playerPosition, const std::vector<PositionComponent>& enemyPositions, const std::vector<PositionComponent>& pillPositions) {
    DrawCircle((int)playerPosition.x, (int)playerPosition.y, 10, YELLOW);
    
    for (const auto& enemy : enemyPositions) {
        DrawCircle((int)enemy.x, (int)enemy.y, 10, RED);
    }
    
    for (const auto& pill : pillPositions) {
        if (pill.x != -100.0f && pill.y != -100.0f) {
            DrawCircle((int)pill.x, (int)pill.y, 5, GREEN);
        }
    }
}

int main() {
    const int screenWidth = GRID_WIDTH * TILE_SIZE;
    const int screenHeight = GRID_HEIGHT * TILE_SIZE;
    InitWindow(screenWidth, screenHeight, "Pac-Man ECS Game");
    
    GenerateMaze(maze);
    //LoadShader("compute_shader.glsl");
    
    std::vector<PositionComponent> pillPositions;
    PlacePills(maze, pillPositions);
    
    PositionComponent playerPosition = {TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
    
    const size_t enemyCount = 3;
    std::vector<PositionComponent> enemyPositions(enemyCount);
    for (size_t i = 0; i < enemyCount; ++i) {
        enemyPositions[i].x = GetRandomValue(1, GRID_WIDTH - 2) * TILE_SIZE;
        enemyPositions[i].y = GetRandomValue(1, GRID_HEIGHT - 2) * TILE_SIZE;
    }
    
    while (!WindowShouldClose()) {
        ParallelMoveEnemies(enemyPositions, playerPosition, maze);

        for (size_t i = 0; i < pillPositions.size(); ++i) {
            if (CheckCollision(playerPosition, pillPositions[i], 10.0f)) {
                pillPositions[i] = {-100.0f, -100.0f};  
            }
        }
        for (size_t i = 0; i < enemyCount; ++i) {
            if (CheckCollision(playerPosition, enemyPositions[i], 10.0f)) {
                DrawText("Game Over!", screenWidth / 2 - 70, screenHeight / 2 - 20, 20, RED);
                EndDrawing();
                CloseWindow();
                return 0;
            }
        }

        if (IsKeyDown(KEY_W) && IsPath(maze, (int)(playerPosition.x / TILE_SIZE), (int)((playerPosition.y - 0.05f) / TILE_SIZE))) {
            playerPosition.y -= 0.05f;
        }
        if (IsKeyDown(KEY_S) && IsPath(maze, (int)(playerPosition.x / TILE_SIZE), (int)((playerPosition.y + 0.05f) / TILE_SIZE))) {
            playerPosition.y += 0.05f;
        }
        if (IsKeyDown(KEY_A) && IsPath(maze, (int)((playerPosition.x - 0.05f) / TILE_SIZE), (int)(playerPosition.y / TILE_SIZE))) {
            playerPosition.x -= 0.05f;
        }
        if (IsKeyDown(KEY_D) && IsPath(maze, (int)((playerPosition.x + 0.05f) / TILE_SIZE), (int)(playerPosition.y / TILE_SIZE))) {
            playerPosition.x += 0.05f;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawMaze(maze);
        DrawEntities(playerPosition, enemyPositions, pillPositions);

        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}