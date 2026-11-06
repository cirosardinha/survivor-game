#include <stdlib.h>

#include "States.h"
#include "config.h"
#include "math.h"
#include "raylib.h"
#include "ui.h"

static void drawHelpRow(
    Font font, Rectangle panel, float y, const char* label, Texture2D* icon,
    const char* text);

void updateMenu(Game* game) {
	if (game->startTimer == 0.0f &&
	    (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
		PlaySound(game->resourceManager->startSound);
		game->startTimer = 0.7f;
	}

	if (game->startTimer > 0.0f) {
		game->startTimer -= GetFrameTime();

		if (game->startTimer <= 0.0f) {
			game->startTimer = 0.0f;

			resetGame(game);
			changeGameState(game, STATE_GAMEPLAY);
		}
	}
}

void drawMenu(Game* game) {
	ResourceManager* rm = game->resourceManager;
	Font font = rm->uiFont;

	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){22, 30, 26, 255});

	drawCenteredLine(font, "SURVIVOR", 65, 90, BLACK);
	drawCenteredLine(font, "SURVIVOR", 60, 90, (Color){255, 203, 70, 255});

	Rectangle panel = {WINDOW_WIDTH / 2.0f - 330, 190, 660, 300};
	DrawRectangleRec(panel, (Color){40, 62, 53, 255});
	DrawRectangleLinesEx(panel, 3, (Color){108, 152, 118, 255});

	drawCenteredLine(font, "Como jogar", 208, 30, YELLOW);

	drawHelpRow(font, panel, 258, "WASD", NULL, "Mover");
	drawHelpRow(font, panel, 312, "AUTO", NULL, "Ataque automático");
	drawHelpRow(font, panel, 366, NULL, &rm->xpGemTexture, "Gemas dão XP");
	drawHelpRow(
	    font, panel, 420, NULL, &rm->healthPickupTexture,
	    "Corações curam a vida");

	if (fmodf((float) GetTime(), 1.2f) < 0.8f) {
		drawCenteredLine(font, "Clique ou ENTER para começar", 540, 30, WHITE);
	}

	int best = getRankingScore(0);
	if (best > 0) {
		drawCenteredLine(
		    font, TextFormat("Recorde: %d", best), 600, 30, YELLOW);
	}
}

static void drawHelpRow(
    Font font, Rectangle panel, float y, const char* label, Texture2D* icon,
    const char* text) {
	float colX = panel.x + 40;
	float colW = 110;
	float textX = colX + colW + 20;

	if (icon != NULL) {
		Rectangle src = {0.0f, 0.0f, 16.0f, 16.0f};
		Rectangle dest = {colX + (colW - 48.0f) / 2.0f, y, 48.0f, 48.0f};
		DrawTexturePro(*icon, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
	} else if (label != NULL) {
		Vector2 size = MeasureTextEx(font, label, 30, 2);
		Vector2 pos = {
		    (float) (int) (colX + (colW - size.x) / 2.0f),
		    (float) (int) (y + (48.0f - size.y) / 2.0f),
		};
		DrawTextEx(font, label, pos, 30, 2, YELLOW);
	}

	Vector2 textPos = {textX, (float) (int) (y + (48.0f - 30.0f) / 2.0f)};
	DrawTextEx(font, text, textPos, 30, 2, WHITE);
}
