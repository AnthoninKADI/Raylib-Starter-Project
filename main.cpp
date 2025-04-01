#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>


#define GRID_WIDTH 15
#define GRID_HEIGHT 15
#define TILE_SIZE 40


struct PositionComponent {
   float x, y;
};


struct PlayerComponent {
   bool isPlayer;
};


enum Tile {
   WALL,
   PATH
};


// Générer un labyrinthe aléatoire (simple)
void GenerateMaze(std::vector<std::vector<Tile>>& maze) {
   srand(time(NULL));


   // Remplir tout le labyrinthe avec des murs
   for (int y = 0; y < GRID_HEIGHT; ++y) {
       for (int x = 0; x < GRID_WIDTH; ++x) {
           maze[y][x] = WALL;
       }
   }


   // Créer un chemin aléatoire
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


// Vérification si la position est un mur ou un chemin
bool IsPath(const std::vector<std::vector<Tile>>& maze, int x, int y) {
   if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
   return maze[y][x] == PATH;
}


// Mouvement des ennemis pour suivre le joueur
void MoveEnemyTowardsPlayer(PositionComponent& enemyPos, const PositionComponent& playerPos, const std::vector<std::vector<Tile>>& maze, float speed) {
   int enemyX = (int)(enemyPos.x / TILE_SIZE);
   int enemyY = (int)(enemyPos.y / TILE_SIZE);
   int playerX = (int)(playerPos.x / TILE_SIZE);
   int playerY = (int)(playerPos.y / TILE_SIZE);


   // Calculer la direction
   int dx = 0, dy = 0;
   if (playerX > enemyX) dx = 1;
   if (playerX < enemyX) dx = -1;
   if (playerY > enemyY) dy = 1;
   if (playerY < enemyY) dy = -1;


   // Déplacement sur l'axe X
   if (IsPath(maze, enemyX + dx, enemyY)) {
       enemyPos.x += dx * speed;
   }


   // Déplacement sur l'axe Y
   if (IsPath(maze, enemyX, enemyY + dy)) {
       enemyPos.y += dy * speed;
   }
}


// Positionner des pills (surtout dans les cases de chemins)
void PlacePills(std::vector<std::vector<Tile>>& maze, std::vector<PositionComponent>& pillPositions) {
   for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
       for (int x = 1; x < GRID_WIDTH - 1; ++x) {
           if (maze[y][x] == PATH && GetRandomValue(0, 10) > 7) {
               pillPositions.push_back({(float)(x * TILE_SIZE + TILE_SIZE / 2), (float)(y * TILE_SIZE + TILE_SIZE / 2)});
           }
       }
   }
}
// Affichage du labyrinthe
void DrawMaze(const std::vector<std::vector<Tile>>& maze) {
   for (int y = 0; y < GRID_HEIGHT; ++y) {
       for (int x = 0; x < GRID_WIDTH; ++x) {
           if (maze[y][x] == WALL) {
               DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);  // Mur
           }
       }
   }
}


// Vérification des collisions
bool CheckCollision(const PositionComponent& playerPos, const PositionComponent& entityPos, float radius) {
   float distX = playerPos.x - entityPos.x;
   float distY = playerPos.y - entityPos.y;
   float distance = sqrt(distX * distX + distY * distY);
   return distance < radius * 2;  // Si la distance entre le joueur et l'entité est inférieure au rayon
}


int main() {
   const int screenWidth = GRID_WIDTH * TILE_SIZE;
   const int screenHeight = GRID_HEIGHT * TILE_SIZE;
   InitWindow(screenWidth, screenHeight, "Pac-Man ECS Game");


   // Créer un labyrinthe
   std::vector<std::vector<Tile>> maze(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));
   GenerateMaze(maze);


   // Créer des pills
   std::vector<PositionComponent> pillPositions;
   PlacePills(maze, pillPositions);


   // Créer le joueur (Pac-Man)
   PositionComponent playerPosition = {TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};  // Initialement à 1.5 cases du coin supérieur gauche


   // Créer des ennemis (Fantômes)
   const size_t enemyCount = 3;
   std::vector<PositionComponent> enemyPositions(enemyCount);
   for (size_t i = 0; i < enemyCount; ++i) {
       enemyPositions[i].x = GetRandomValue(1, GRID_WIDTH - 2) * TILE_SIZE;
       enemyPositions[i].y = GetRandomValue(1, GRID_HEIGHT - 2) * TILE_SIZE;
   }


   // Boucle de jeu
   while (!WindowShouldClose()) {
       // Déplacer les ennemis pour suivre Pac-Man
       for (size_t i = 0; i < enemyCount; ++i) {
           MoveEnemyTowardsPlayer(enemyPositions[i], playerPosition, maze, 0.05f);
       }


       // Vérification des collisions avec les pills
       for (size_t i = 0; i < pillPositions.size(); ++i) {
           if (CheckCollision(playerPosition, pillPositions[i], 10.0f)) {
               pillPositions[i] = {-100.0f, -100.0f};  // Supprimer la pill après collision
           }
       }
      
       // Vérification des collisions avec les fantômes (Game Over)
       for (size_t i = 0; i < enemyCount; ++i) {
           if (CheckCollision(playerPosition, enemyPositions[i], 10.0f)) {
               DrawText("Game Over!", screenWidth / 2 - 70, screenHeight / 2 - 20, 20, RED);
               EndDrawing();  // Afficher "Game Over" une dernière fois
               CloseWindow(); // Fermer la fenêtre après la détection de la collision
               return 0;  // Terminer le programme
           }
       }


       // Contrôle du joueur
       if (IsKeyDown(KEY_W) && IsPath(maze, (int)(playerPosition.x / TILE_SIZE), (int)((playerPosition.y - 0.05f) / TILE_SIZE))) {
           playerPosition.y -= 0.05f;  // Déplacer vers le haut
       }
       if (IsKeyDown(KEY_S) && IsPath(maze, (int)(playerPosition.x / TILE_SIZE), (int)((playerPosition.y + 0.05f) / TILE_SIZE))) {
           playerPosition.y += 0.05f;  // Déplacer vers le bas
       }
       if (IsKeyDown(KEY_A) && IsPath(maze, (int)((playerPosition.x - 0.05f) / TILE_SIZE), (int)(playerPosition.y / TILE_SIZE))) {
           playerPosition.x -= 0.05f;  // Déplacer vers la gauche
       }
       if (IsKeyDown(KEY_D) && IsPath(maze, (int)((playerPosition.x + 0.05f) / TILE_SIZE), (int)(playerPosition.y / TILE_SIZE))) {
           playerPosition.x += 0.05f;  // Déplacer vers la droite
       }


       // Affichage du jeu
       BeginDrawing();
       ClearBackground(GRAY);
       // Affichage du labyrinthe
       DrawMaze(maze);


       // Affichage du joueur (Pac-Man)
       DrawCircle((int)playerPosition.x, (int)playerPosition.y, 10, YELLOW);


       // Affichage des ennemis (fantômes)
       for (size_t i = 0; i < enemyCount; ++i) {
           DrawCircle((int)enemyPositions[i].x, (int)enemyPositions[i].y, 10, RED);
       }
       // Affichage des pills
       for (size_t i = 0; i < pillPositions.size(); ++i) {
           if (pillPositions[i].x != -100.0f && pillPositions[i].y != -100.0f) {  // Si pill n'a pas été mangée
               DrawCircle((int)pillPositions[i].x, (int)pillPositions[i].y, 5, GREEN);
           }
       }


       EndDrawing();
   }


   CloseWindow();
   return 0;
}