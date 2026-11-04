#include "ui.h"
#include "raylib.h"

void drawPlayerHealth(const Player* player, const ResourceManager* rm) {
	float x = GetScreenWidth() * 0.02f;
	float y = GetScreenHeight() * 0.03f;

	float percent = (float) player->health / player->maxHealth;
	if (percent < 0.0f) percent = 0.0f;

	DrawTextureEx(rm->hpFrameTexture, (Vector2){x, y}, 0.0f, UI_SCALE, WHITE);

	int fillWidth = (int) (rm->hpFillTexture.width * percent);
	Rectangle src = {
	    0.0f, 0.0f, (float) fillWidth, (float) rm->hpFillTexture.height};
	Rectangle dest = {
	    x + 25 * UI_SCALE,
	    y + 9 * UI_SCALE,
	    fillWidth * UI_SCALE,
	    rm->hpFillTexture.height * UI_SCALE,
	};
	DrawTexturePro(
	    rm->hpFillTexture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void drawTextInFrame(
    const ResourceManager* rm, const char* text, Vector2 frameOrigin,
    int frameHeight, float areaX, float areaWidth, float scale) {
	float fontSize = 15 * scale;
	float spacing = scale;
	Vector2 size = MeasureTextEx(rm->uiFont, text, fontSize, spacing);

	Vector2 pos = {
	    frameOrigin.x + (areaX + areaWidth / 2.0f) * scale - size.x / 2.0f,
	    frameOrigin.y + frameHeight * scale / 2.0f - size.y / 2.0f,
	};
	pos.x = (float) (int) pos.x;
	pos.y = (float) (int) pos.y;

	DrawTextEx(rm->uiFont, text, pos, fontSize, spacing, WHITE);
}

void drawTimer(float timer, const ResourceManager* rm) {
	Texture2D frame = rm->timerFrameTexture;

	Vector2 origin = {
	    (GetScreenWidth() - frame.width * PLATE_SCALE) / 2.0f,
	    GetScreenHeight() * 0.03f,
	};
	DrawTextureEx(frame, origin, 0.0f, PLATE_SCALE, WHITE);

	int minutes = (int) timer / 60;
	int seconds = (int) timer % 60;
	drawTextInFrame(
	    rm, TextFormat("%02d:%02d", minutes, seconds), origin, frame.height, 33,
	    53, PLATE_SCALE);
}

void drawScore(const Player* player, const ResourceManager* rm) {
	Texture2D frame = rm->scoreFrameTexture;

	float margin = GetScreenWidth() * 0.02f;
	Vector2 origin = {
	    GetScreenWidth() - frame.width * PLATE_SCALE - margin,
	    GetScreenHeight() * 0.03f,
	};
	DrawTextureEx(frame, origin, 0.0f, PLATE_SCALE, WHITE);

	drawTextInFrame(
	    rm, TextFormat("%d", player->score), origin, frame.height, 31, 72,
	    PLATE_SCALE);
}

void drawPlayerLevel(const Player* player, const ResourceManager* rm) {
	float x = GetScreenWidth() * 0.02f;

	float y = GetScreenHeight() * 0.03f + (25 + 1) * UI_SCALE;

	DrawTextureEx(rm->xpFrameTexture, (Vector2){x, y}, 0.0f, UI_SCALE, WHITE);

	int fillWidth =
	    (int) (rm->xpFillTexture.width * getPlayerXpProgress(player));
	Rectangle src = {
	    0.0f, 0.0f, (float) fillWidth, (float) rm->xpFillTexture.height};
	Rectangle dest = {
	    x + 25 * UI_SCALE,
	    y + 8 * UI_SCALE,
	    fillWidth * UI_SCALE,
	    rm->xpFillTexture.height * UI_SCALE,
	};
	DrawTexturePro(
	    rm->xpFillTexture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

	DrawTextEx(
	    rm->uiFont, TextFormat("Nivel %d", player->level),
	    (Vector2){x, y + 24 * UI_SCALE + 6}, 30, 2, WHITE);

	if (player->levelUpTimer > 0.0f) {
		const char* text = "LEVEL UP!";
		float fontSize = 50;
		float spacing = 4;

		Vector2 size = MeasureTextEx(rm->uiFont, text, fontSize, spacing);
		DrawTextEx(
		    rm->uiFont, text,
		    (Vector2){
		        (GetScreenWidth() - size.x) / 2.0f, GetScreenHeight() / 3.0f},
		    fontSize, spacing, YELLOW);
	}
}
