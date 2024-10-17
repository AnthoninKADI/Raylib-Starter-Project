#include "raylib.h"
#include "Node.h"  // Inclure la structure Node ici
#include <vector>

class Citizen
{
public:
    Citizen(int startX, int startY, float size, Color color);
    
    void Update(const std::vector<std::vector<Node>>& grid);
    void Draw() const;

private:
    Vector2 position;
    float size;
    Color color;

    bool CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid);
};


