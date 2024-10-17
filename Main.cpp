#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "raylib.h"
#include "Citizen.h"
#include "Node.h"

const int screenWidth = 800;
const int screenHeight = 600;
const int rows = 20;
const int cols = 20;
const int rectWidth = 50;
const int rectHeight = 50;
const int padding = 1;

float importScale = 0.4f;

Color normalColor = WHITE;
Color difficultColor = BROWN;
Color challengingColor = ORANGE;
Color roadColor = BLACK;
Color pathColor = SKYBLUE;
Color emptyColor = DARKGRAY;

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

void DrawPathWithGrid(const std::vector<std::vector<Node>>& grid, const std::vector<Node*>& path, Texture2D blueShop, Texture2D redShop, Texture2D purpleShop, Texture2D blueHouse, Texture2D redHouse, Texture2D purpleHouse)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            const Node& node = grid[j][i];

            if (node.terrain == BlueShop) 
            {
                DrawTextureEx(blueShop, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (node.terrain == RedShop) 
            {
                DrawTextureEx(redShop, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (node.terrain == PurpleShop) 
            {
                DrawTextureEx(purpleShop, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (node.terrain == BlueHouse) 
            {
                DrawTextureEx(blueHouse, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (node.terrain == RedHouse) 
            {
                DrawTextureEx(redHouse, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (node.terrain == PurpleHouse) 
            {
                DrawTextureEx(purpleHouse, 
                              {(float)(j * rectWidth), (float)(i * rectHeight)}, 
                              0.0f, 
                              importScale, 
                              WHITE); 
            }
            else if (std::find(path.begin(), path.end(), &node) != path.end())
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, pathColor);
            }
            else if (node.obstacle || node.terrain == Road)
            {
                DrawRectangle(j * rectWidth, i * rectHeight, rectWidth - 1, rectHeight - 1, roadColor);
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
    InitWindow(screenWidth, screenHeight, "City Sim");
    SetTargetFPS(60);
    
    Texture2D blueShop = LoadTexture("resources/BlueShop.png");
    Texture2D redShop = LoadTexture("resources/RedShop.png");
    Texture2D purpleShop = LoadTexture("resources/PurpleShop.png");
    
    Texture2D blueHouse = LoadTexture("resources/BlueHouse.png");
    Texture2D redHouse = LoadTexture("resources/RedHouse.png");
    Texture2D purpleHouse = LoadTexture("resources/PurpleHouse.png");

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
                grid[i][j].terrain = Road;
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

    grid[1][1].obstacle = true;
    grid[1][2].obstacle = true;
    grid[1][3].obstacle = true;
    grid[1][4].obstacle = true;
    grid[1][5].obstacle = true;
    grid[1][6].obstacle = true;
    grid[1][7].obstacle = true;
    grid[1][8].obstacle = true;
    grid[1][9].obstacle = true;
    grid[1][10].obstacle = true;

    grid[2][1].obstacle = true;
    grid[3][1].obstacle = true;
    grid[4][1].obstacle = true;
    grid[5][1].obstacle = true;
    grid[6][1].obstacle = true;
    grid[7][1].obstacle = true;
    grid[7][2].obstacle = true;
    grid[7][3].obstacle = true;
    grid[7][4].obstacle = true;
    grid[7][6].obstacle = true;
    grid[7][7].obstacle = true;
    grid[7][8].obstacle = true;
    grid[7][9].obstacle = true;

    grid[2][5].obstacle = true;
    grid[3][5].obstacle = true;
    grid[4][5].obstacle = true;
    grid[5][5].obstacle = true;
    grid[6][5].obstacle = true;
    grid[7][5].obstacle = true;
    grid[8][5].obstacle = true;
    grid[9][5].obstacle = true;
    grid[10][5].obstacle = true;
    grid[11][5].obstacle = true;
    grid[12][5].obstacle = true;
    grid[13][5].obstacle = true;
    grid[14][5].obstacle = true;

    grid[14][6].obstacle = true;
    grid[14][7].obstacle = true;
    grid[14][8].obstacle = true;
    grid[14][9].obstacle = true;
    grid[14][10].obstacle = true;

    grid[2][10].obstacle = true;
    grid[3][10].obstacle = true;
    grid[4][10].obstacle = true;
    grid[5][10].obstacle = true;
    grid[6][10].obstacle = true;
    grid[7][10].obstacle = true;
    grid[8][10].obstacle = true;
    grid[9][10].obstacle = true;
    grid[10][10].obstacle = true;
    grid[11][10].obstacle = true;
    grid[12][10].obstacle = true;
    grid[13][10].obstacle = true;

    grid[2][0].terrain = BlueShop; 
    grid[15][7].terrain = RedShop; 
    grid[10][4].terrain = PurpleShop; 
    grid[9][9].terrain = BlueHouse; 
    grid[3][4].terrain = RedHouse; 
    grid[2][7].terrain = PurpleHouse;

    std::vector<Citizen> citizens; //
    citizens.emplace_back(2, 0, 7.0f, RED);   // Citoyen à la position (2,0) avec taille 10, couleur rouge
    citizens.emplace_back(14, 6, 7.0f, BLUE); //
    citizens.emplace_back(9, 10, 7.0f, PINK);

    while (!WindowShouldClose())
    {

        for (auto& citizen : citizens)  //
        {                               //
            citizen.Update(grid);       //
        }                               //
        
        BeginDrawing();
        ClearBackground(emptyColor);

        DrawPathWithGrid(grid, {}, blueShop, redShop, purpleShop, blueHouse, redHouse, purpleHouse);

        // Dessin des citoyens
        for (const auto& citizen : citizens)    //
        {                                       //
            citizen.Draw();                     //
        }                                       //
        
        EndDrawing();
    }
    
    UnloadTexture(blueShop);
    UnloadTexture(redShop);
    UnloadTexture(purpleShop);
    UnloadTexture(blueHouse);
    UnloadTexture(redHouse);
    UnloadTexture(purpleHouse);
    CloseWindow();

    return 0;
}
