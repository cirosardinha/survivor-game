#include "ui.h"
#include "raylib.h"

void drawPlayerHealth(const Player* player) {
	int barWidth = 200;
	int barHeight = 20;

	int marginX = GetScreenWidth() * 0.02f;
	int marginY = GetScreenHeight() * 0.03f;

	float healthPercent = (float) player->health / player->maxHealth;

	int currentWidth = (int) (barWidth * healthPercent);

	DrawRectangle(marginX, marginY, barWidth, barHeight, DARKGRAY);

	DrawRectangle(marginX, marginY, currentWidth, barHeight, GREEN);

	DrawRectangleLines(marginX, marginY, barWidth, barHeight, WHITE);
}