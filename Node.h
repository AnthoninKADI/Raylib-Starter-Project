#ifndef NODE_H
#define NODE_H

#include "raylib.h"
#include <vector>
#include <cmath>

enum Terrain
{
    Normal,
    Challenging,
    Difficult,
    BlueShop,
    RedShop,
    PurpleShop,
    BlueHouse,
    RedHouse,
    PurpleHouse,
    Road
};

struct Node
{
    int x, y;
    float g, h, f;
    bool obstacle;
    Node* parent;
    Terrain terrain;
    float CostMultiplier;
    
    bool operator==(const Node& other) const
    {
        return x == other.x && y == other.y;
    }
};

float distance(const Node& node1, const Node& node2);
float getTerrainCost(const Node& node);

#endif // NODE_H
