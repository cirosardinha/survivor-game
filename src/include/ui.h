#pragma once

#include "Player.h"
#include "ResourceManager.h"
#include "raylib.h"

void drawPlayerHealth(const Player* player, const ResourceManager* rm);
void drawTimer(float timer, const ResourceManager* rm);
void drawScore(const Player* player, const ResourceManager* rm);
void drawPlayerLevel(const Player* player, const ResourceManager* rm);
void drawCenteredLine(
    Font font, const char* text, int y, int size, Color color);
void drawButton(Font font, Rectangle button, const char* label);
