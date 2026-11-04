#pragma once

#include "Player.h"
#include "ResourceManager.h"

void drawPlayerHealth(const Player* player, const ResourceManager* rm);
void drawTimer(float timer, const ResourceManager* rm);
void drawScore(const Player* player, const ResourceManager* rm);
void drawPlayerLevel(const Player* player, const ResourceManager* rm);