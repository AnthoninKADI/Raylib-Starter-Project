#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>
#include <atomic>
#include <functional>


#define GRID_WIDTH 15
#define GRID_HEIGHT 15
#define TILE_SIZE 40


// ECS Components
struct PositionComponent {
   float x, y;
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


// Système ECS
class System {
public:
   virtual void Update(float deltaTime) = 0;
};


class MovementSystem : public System {
   std::vector<PositionComponent>& positions;
   std::vector<EnemyComponent>& enemies;
public:
   MovementSystem(std::vector<PositionComponent>& pos, std::vector<EnemyComponent>& en) : positions(pos), enemies(en) {}


   void Update(float deltaTime) override {
       for (size_t i = 0; i < enemies.size(); ++i) {
           // Déplacer les ennemis (trivial ici pour l'exemple)
           positions[i].x += 1.0f * deltaTime;
           positions[i].y += 1.0f * deltaTime;
       }
   }
};


// Job System pour exécuter des tâches en parallèle
void ParallelJob(std::function<void()> job) {
   std::thread t(job);
   t.detach();  // Exécuter la tâche en parallèle
}


// Shader Setup
const char* computeShaderSrc = R"(
   #version 430 core
   layout(local_size_x = 64) in;
   layout(binding = 0) buffer VertexData {
       vec4 vertices[];
   };
  
   void main() {
       uint id = gl_GlobalInvocationID.x;
       vertices[id] = vec4(id, id * 2.0f, 0.0f, 1.0f);  // Remplir le buffer avec des données simples pour l'exemple
   }
)";


// Générer un labyrinthe aléatoire (simple)
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


// Vérification des collisions
bool CheckCollision(const PositionComponent& playerPos, const PositionComponent& entityPos, float radius) {
   float distX = playerPos.x - entityPos.x;
   float distY = playerPos.y - entityPos.y;
   float distance = sqrt(distX * distX + distY * distY);
   return distance < radius * 2;
}


int main() {
   const int screenWidth = GRID_WIDTH * TILE_SIZE;
   const int screenHeight = GRID_HEIGHT * TILE_SIZE;
   InitWindow(screenWidth, screenHeight, "Pac-Man ECS Game");


   // Création d'un labyrinthe
   std::vector<std::vector<Tile>> maze(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));
   GenerateMaze(maze);


   // Position des entités
   std::vector<PositionComponent> positions(10);
   std::vector<EnemyComponent> enemies(3);
   std::vector<PillComponent> pills;


   // Définir des systèmes ECS
   MovementSystem movementSystem(positions, enemies);


   // Charger le compute shader
   Shader computeShader = LoadShader(nullptr, computeShaderSrc);


   // Initialiser des entités (exemple simplifié)
   for (size_t i = 0; i < 3; ++i) {
       positions[i].x = 100.0f * i;
       positions[i].y = 100.0f * i;
   }


   // Job system pour mettre à jour les ennemis en parallèle
   ParallelJob([&]() {
       movementSystem.Update(0.016f);  // Mettez à jour les ennemis en parallèle
   });


   // Boucle principale du jeu
   while (!WindowShouldClose()) {
       // Contrôles du joueur et logique du jeu (pas de changement pour l'instant)


       // Affichage du jeu
       BeginDrawing();
       ClearBackground(GRAY);


       // Afficher le labyrinthe
       for (int y = 0; y < GRID_HEIGHT; ++y) {
           for (int x = 0; x < GRID_WIDTH; ++x) {
               if (maze[y][x] == WALL) {
                   DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
               }
           }
       }


       // Afficher le joueur (Pac-Man)
       DrawCircle((int)positions[0].x, (int)positions[0].y, 10, YELLOW);


       // Afficher les ennemis
       for (size_t i = 0; i < 3; ++i) {
           DrawCircle((int)positions[i].x, (int)positions[i].y, 10, RED);
       }


       EndDrawing();
   }


   CloseWindow(); // Fermer la fenêtre
   return 0;
}

