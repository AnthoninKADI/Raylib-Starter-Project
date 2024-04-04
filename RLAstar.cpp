#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "raylib.h"

const int screenWidth = 800;
const int screenHeight = 600;
const int rows = 10;
const int cols = 10;
const int rectWidth = 50;
const int rectHeight = 50;
const int padding = 2;


Color normalColor = WHITE;
Color difficultColor = BROWN;
Color challengingColor = ORANGE;
Color obstacleColor = DARKGRAY;
Color pathColor = SKYBLUE;
Color emptyColor = BLACK;


enum Terrain
{
    Normal,
    Challenging,
    Difficult,
    Obstacle
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
    case Obstacle:
        return std::numeric_limits<float>::infinity();
    }

}

std::vector<Node*> aStar(Node* start, const Node* goal, std::vector<std::vector<Node>>& grid)
{
    start->terrain = Normal;
    start->CostMultiplier = getTerrainCost(*start);

    std::vector<Node*> openList, closedList;
    start->g = 0;
    start->h = distance(*start, *goal);
    start->f = start->g + start->h;
    openList.push_back(start);

    while (!openList.empty())
    {
        auto currentNode = std::min_element(openList.begin(), openList.end(), [](const Node* a, const Node* b)
            {
                return a->f < b->f;
            });
        Node* current = *currentNode;
        openList.erase(currentNode);

        closedList.push_back(current);

        if (*current == *goal)
        {
            std::vector<Node*> path;
            while (current != nullptr)
            {
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                {
                    continue;
                }
                const int newX = current->x + dx;
                const int newY = current->y + dy;

                if (newX < 0 || newX >= rows || newY < 0 || newY >= cols)
                {
                    continue;
                }

                Node& child = grid[newX][newY];

                if (child.obstacle)
                {
                    continue;
                }

                if (std::find(closedList.begin(), closedList.end(), &child) != closedList.end())
                {
                    continue;
                }

                const float tentative_g = current->g + distance(*current, child) * child.CostMultiplier;
                bool isNewPath = false;

                auto it = find(openList.begin(), openList.end(), &child);
                if (it == openList.end())
                {
                    isNewPath = true;
                    child.h = distance(child, *goal);
                    openList.push_back(&child);
                }
                else if (tentative_g < child.g)
                {
                    isNewPath = true;
                }

                if (isNewPath)
                {
                    child.parent = current;
                    child.g = tentative_g;
                    child.f = child.g + child.h;
                }
            }
        }
    }
    return {};
}

void DrawPathWithGrid(const std::vector<std::vector<Node>>& grid, const std::vector<Node*>& path)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            const Node& node = grid[j][i];
            if (std::find(path.begin(), path.end(), &node) != path.end())
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, pathColor);
            }
            else if (node.obstacle || node.terrain == Obstacle)
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, obstacleColor);
            }
            else if (node.terrain == Normal)
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, normalColor);
            }
            else if (node.terrain == Challenging)
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, challengingColor);
            }
            else if (node.terrain == Difficult)
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, difficultColor);
            }
            else
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, emptyColor);
            }
        }
    }
}

bool startSelected = false;
bool endSelected = false;

int main()
{
    InitWindow(screenWidth, screenHeight, "Astar Raylib");
    SetTargetFPS(60);

    std::vector<std::vector<Node>> grid(rows, std::vector<Node>(cols));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            grid[i][j].x = i;
            grid[i][j].y = j;
            grid[i][j].parent = nullptr;

            if (grid[i][j].obstacle)
            {
                grid[i][j].terrain = Obstacle;
            }
            else
            {
                grid[i][j].terrain = Normal;
            }
            grid[i][j].CostMultiplier = getTerrainCost(grid[i][j]);
        }
    }

    Node* start = grid[0].data();
    Node* goal = &grid[rows - 1][cols - 1];

    grid[1][6].obstacle = true;
    grid[1][7].obstacle = true;
    grid[1][8].obstacle = true;

    grid[2][1].obstacle = true;
    grid[2][2].obstacle = true;
    grid[2][3].terrain = Challenging;
    grid[2][4].obstacle = true;
    grid[2][5].terrain = Challenging;

    grid[4][5].obstacle = true;
    grid[4][7].terrain = Difficult;
    grid[4][8].terrain = Difficult;
    grid[4][9].obstacle = true;

    grid[6][6].terrain = Challenging;
    grid[6][7].obstacle = true;
    grid[6][9].terrain = Difficult;

    grid[8][5].obstacle = true;
    grid[8][6].terrain = Challenging;
    grid[8][7].obstacle = true;

    grid[5][5].obstacle = true;

    grid[6][0].obstacle = true;
    grid[6][1].terrain = Difficult;
    grid[6][2].obstacle = true;

    grid[5][3].obstacle = true;

    grid[8][2].obstacle = true;
    grid[8][3].terrain = Difficult;


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(emptyColor);

        if (!start) startSelected = false;
        if (!goal) endSelected = false;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))

        {
            const Vector2 mousePos = GetMousePosition();

            const int x = static_cast<int>(mousePos.x) / rectWidth;

            const int y = static_cast<int>(mousePos.y) / rectHeight;

            if (x >= 0 && x < rows && y >= 0 && y < cols)

            {

                if (!startSelected)

                {
                    start = &grid[x][y];
                    startSelected = true;
                    printf("start ");

                }

                else if (!endSelected && &grid[x][y] != start)

                {
                    goal = &grid[x][y];
                    endSelected = true;
                    printf(" end \n");

                }

            }
        }
        DrawPathWithGrid(grid, {});

        if (startSelected && endSelected && !start->obstacle && !goal->obstacle)

        {
            const std::vector<Node*> path = aStar(start, goal, grid);
            // Print 
            DrawPathWithGrid(grid, path);

            if (start != nullptr) DrawRectangle(start->x * rectWidth, start->y * rectHeight, rectWidth - 1, rectHeight - 1, GREEN);

            if (goal != nullptr) DrawRectangle(goal->x * rectWidth, goal->y * rectHeight, rectWidth - 1, rectHeight - 1, RED);

        }
        EndDrawing();
    }

    CloseWindow();
    return 0;

}