#pragma once

#include "Player.h"

#define UPGRADE_CHOICES 3

typedef enum UpgradeType {
	UPGRADE_MAX_HEALTH,
	UPGRADE_DAMAGE,
	UPGRADE_ATTACK_SPEED,
	UPGRADE_MOVE_SPEED,
	UPGRADE_PICKUP_RANGE,
	UPGRADE_HEAL,
	UPGRADE_ORB,
	UPGRADE_COUNT
} UpgradeType;

const char* getUpgradeName(UpgradeType type);
const char* getUpgradeDescription(UpgradeType type);
void applyUpgrade(Player* player, UpgradeType type);
void rollUpgrades(const Player* player, UpgradeType* choices, int count);