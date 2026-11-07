#pragma once

#include "Enemy.h"
#include "Player.h"

#define ATTACK_WINDUP_TIME 0.45f
#define ATTACK_HIT_TOLERANCE 1.35f
#define ATTACK_CONE_ANGLE 60.0f

void updatePlayerAttack(Player* player, Enemy* enemies, int count, float delta);
void updateEnemyAttack(Enemy* enemy, Player* player, float delta);