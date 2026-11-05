#include "Game.h"
#include "States.h"
#include "Upgrades.h"
#include "config.h"
#include "ui.h"

#define CARD_WIDTH 280
#define CARD_HEIGHT 230
#define CARD_GAP 40
#define CARD_Y 250

static Rectangle cardRect(int index);
static void drawCardText(
    Font font, const char* text, Rectangle card, float y, float size,
    Color color);

void enterLevelUp(Game* game) {
	rollUpgrades(game->upgradeChoices, UPGRADE_CHOICES);
	changeGameState(game, STATE_LEVELUP);
}

void updateLevelUp(Game* game) {
	int chosen = -1;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mouse = GetMousePosition();

		for (int i = 0; i < UPGRADE_CHOICES; i++) {
			if (CheckCollisionPointRec(mouse, cardRect(i))) chosen = i;
		}
	}

	if (IsKeyPressed(KEY_ONE)) chosen = 0;
	if (IsKeyPressed(KEY_TWO)) chosen = 1;
	if (IsKeyPressed(KEY_THREE)) chosen = 2;

	if (chosen < 0) return;

	applyUpgrade(&game->player, game->upgradeChoices[chosen]);
	game->player.pendingLevelUps--;

	if (game->player.pendingLevelUps > 0) {
		rollUpgrades(game->upgradeChoices, UPGRADE_CHOICES);
	} else {
		changeGameState(game, STATE_GAMEPLAY);
	}
}

void drawLevelUp(Game* game) {
	drawGameplay(game);

	Font font = game->resourceManager->uiFont;
	Vector2 mouse = GetMousePosition();

	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 170});

	drawCenteredLine(
	    font, TextFormat("NIVEL %d!", game->player.level), 80, 60, YELLOW);
	drawCenteredLine(font, "Escolha um bonus", 160, 30, WHITE);

	for (int i = 0; i < UPGRADE_CHOICES; i++) {
		Rectangle card = cardRect(i);
		UpgradeType type = game->upgradeChoices[i];

		bool hover = CheckCollisionPointRec(mouse, card);
		Color fill =
		    hover ? (Color){62, 96, 80, 255} : (Color){40, 62, 53, 255};

		DrawRectangleRec(card, fill);
		DrawRectangleLinesEx(card, 3, (Color){108, 152, 118, 255});

		drawCardText(font, TextFormat("%d", i + 1), card, 16, 30, YELLOW);
		drawCardText(font, getUpgradeName(type), card, 70, 36, WHITE);
		drawCardText(
		    font, getUpgradeDescription(type), card, 130, 24,
		    (Color){200, 210, 205, 255});
	}

	drawCenteredLine(
	    font, "Clique no bonus ou aperte 1, 2 ou 3", 520, 24, WHITE);
}

static Rectangle cardRect(int index) {
	float total =
	    UPGRADE_CHOICES * CARD_WIDTH + (UPGRADE_CHOICES - 1) * CARD_GAP;
	float left = (WINDOW_WIDTH - total) / 2.0f;

	return (Rectangle){
	    left + index * (CARD_WIDTH + CARD_GAP), CARD_Y, CARD_WIDTH,
	    CARD_HEIGHT};
}

static void drawCardText(
    Font font, const char* text, Rectangle card, float y, float size,
    Color color) {
	float spacing = (float) ((int) size / 15);
	Vector2 textSize = MeasureTextEx(font, text, size, spacing);

	Vector2 pos = {
	    (float) (int) (card.x + (card.width - textSize.x) / 2.0f),
	    (float) (int) (card.y + y),
	};

	DrawTextEx(font, text, pos, size, spacing, color);
}