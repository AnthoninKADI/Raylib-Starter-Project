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
    const Node* destination; // Destination node pointer
    float speed = 1.0f;      // Speed of the citizen

    bool CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid);
};

#endif // CITIZEN_H
