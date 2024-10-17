#include "Node.h"

// Définitions des fonctions
float distance(const Node& node1, const Node& node2)
{
    return sqrt(pow(node1.x - node2.x, 2) + pow(node1.y - node2.y, 2));
}

float getTerrainCost(const Node& node)
{
    switch (node.terrain)
    {
    case Normal:
        return 1.0f;
    case Challenging:
        return 1.5f;
    case Difficult:
        return 2.0f;
    case BlueShop:
    case RedShop:
    case PurpleShop:
    case BlueHouse:
    case RedHouse:
    case PurpleHouse:
        return 1.0f;
    case Road:
        return std::numeric_limits<float>::infinity();
    }
}
