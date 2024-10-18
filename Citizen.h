#ifndef CITIZEN_H
#define CITIZEN_H

#include "raylib.h"
#include "Node.h"
#include <vector>

class Citizen
{
public:
    Citizen(int startX, int startY, float size, Color color, const std::vector<Node*>& destinations);

    void Update(const std::vector<std::vector<Node>>& grid);
    void Draw() const;

private:
    Vector2 position;
    float size;
    Color color;
    std::vector<Node*> destinations; 
    int currentDestinationIndex;      
    float speed;                      

    void SetNewDestination();          
    bool CanMoveTo(int x, int y, const std::vector<std::vector<Node>>& grid);
};

#endif // CITIZEN_H
