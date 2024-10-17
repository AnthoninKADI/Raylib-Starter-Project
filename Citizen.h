#ifndef CITIZEN_H
#define CITIZEN_H

#include "raylib.h"
#include "Node.h"
#include <vector>

class Citizen
{
public:
    Citizen(int startX, int startY, float size, Color color, const Node* destination);

    void Update(const std::vector<std::vector<Node>>& grid);
    void Draw() const;

private:
    Vector2 position;
    float size;
    Color color;
    Vector2 destination;  // Destination vers laquelle se déplacer
    float speed;          // Vitesse de déplacement

    bool CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid);
};

#endif // CITIZEN_H
