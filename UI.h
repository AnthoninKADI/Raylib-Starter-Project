#pragma once
#include "Player.h"
#include "raylib.h"

class Game;


void DrawCrosshair(int size, int thickness);
void DrawMinimap(const Game& game);
void DrawDamageFlash(const Player& player);