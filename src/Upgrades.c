#include "Upgrades.h"

#include "raylib.h"

#define UPGRADE_HEALTH_BONUS 20
#define UPGRADE_DAMAGE_BONUS 4
#define UPGRADE_COOLDOWN_FACTOR 0.9f
#define UPGRADE_SPEED_FACTOR 1.1f
#define UPGRADE_PICKUP_FACTOR 1.3f
#define UPGRADE_HEAL_AMOUNT 40

#define MIN_ATTACK_COOLDOWN 0.4f
#define MAX_MOVE_SPEED 400.0f

typedef struct UpgradeInfo {
	const char* name;
	const char* description;
} UpgradeInfo;

// Mantenha as descrições iguais aos valores das constantes acima.
static const UpgradeInfo upgradeInfo[UPGRADE_COUNT] = {
    [UPGRADE_MAX_HEALTH] = {"Vitalidade", "+20 de vida\nmaxima"},
    [UPGRADE_DAMAGE] = {"Forca", "+4 de dano"},
    [UPGRADE_ATTACK_SPEED] = {"Agilidade", "Ataque 10%\nmais rapido"},
    [UPGRADE_MOVE_SPEED] = {"Velocidade", "Movimento 10%\nmais rapido"},
    [UPGRADE_PICKUP_RANGE] = {"Coleta", "Alcance de coleta\n30% maior"},
    [UPGRADE_HEAL] = {"Cura", "Recupera 40\nde vida"},
};

const char* getUpgradeName(UpgradeType type) {
	return upgradeInfo[type].name;
}

const char* getUpgradeDescription(UpgradeType type) {
	return upgradeInfo[type].description;
}

void applyUpgrade(Player* player, UpgradeType type) {
	switch (type) {
		case UPGRADE_MAX_HEALTH:
			player->maxHealth += UPGRADE_HEALTH_BONUS;
			healPlayer(player, UPGRADE_HEALTH_BONUS);
			break;

		case UPGRADE_DAMAGE:
			player->attackDamage += UPGRADE_DAMAGE_BONUS;
			break;

		case UPGRADE_ATTACK_SPEED:
			player->attackCooldown *= UPGRADE_COOLDOWN_FACTOR;
			if (player->attackCooldown < MIN_ATTACK_COOLDOWN) {
				player->attackCooldown = MIN_ATTACK_COOLDOWN;
			}
			break;

		case UPGRADE_MOVE_SPEED:
			player->speed *= UPGRADE_SPEED_FACTOR;
			if (player->speed > MAX_MOVE_SPEED) {
				player->speed = MAX_MOVE_SPEED;
			}
			break;

		case UPGRADE_PICKUP_RANGE:
			player->pickupRange *= UPGRADE_PICKUP_FACTOR;
			break;

		case UPGRADE_HEAL:
			healPlayer(player, UPGRADE_HEAL_AMOUNT);
			break;

		default:
			break;
	}
}

void rollUpgrades(UpgradeType* choices, int count) {
	for (int i = 0; i < count; i++) {
		bool repeated;

		do {
			choices[i] = (UpgradeType) GetRandomValue(0, UPGRADE_COUNT - 1);

			repeated = false;
			for (int j = 0; j < i; j++) {
				if (choices[j] == choices[i]) repeated = true;
			}
		} while (repeated);
	}
}