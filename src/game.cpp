#include <ctime>
#include <SDL_scancode.h>
#include <SDL.h>
#include "surface.h"
#include "text.h"
#include "game.h"
#include "scores.h"

namespace Tmpl8 {

#define BG_ASSET "assets/RollingBall/background_green.png"
#define BG_TILE_SIZE 50		
#define TEXT_SCALE 4

#define BALL_BOUNCE_SCALE 1
#define MAX_STARS 10
#define STAR_MAX_VEL 0.5f
#define TOTAL_TIME 60
#define STAR_SPAWN_DELAY 2
#define SPAWN_DIST 100
#define SPAWN_MARGIN 100

Sprite bgSprite(new Surface(BG_ASSET), 1);
Sprite ballSprite(new Surface(BALL_ASSET), 1);

char nameBuff[10] = "";
Scores top3 = Scores();

Game::Game(Surface* screen): screen(screen) {
	Reset(STATE_STARTMENU);

	// temporary fake scores for showing-off's sake
	clearScores();
	saveScore(new char[NAME_LEN] { "sjver" }, 23);
	saveScore(new char[NAME_LEN] { "buasboi" }, 8);
}

void Game::Reset(int newState) {
	state = (enum State)newState;
	ball = new Ball(screen);
	stars.clear();
	score = 0;

	lastSpawnTime = time(NULL);
	startTime = time(NULL);
}

void Game::SpawnStar(uint attempt = 0) {
	Star* star = new Star(screen, vec2{0});
	vec2 monSize = GetMonitorSize();

	star->pos.x = Rand(monSize.x - star->size.x - 2 * SPAWN_MARGIN) + SPAWN_MARGIN;
	star->pos.y = Rand(monSize.y - star->size.y - 2 * SPAWN_MARGIN) + SPAWN_MARGIN;

	// don't recurse infinitely
	if (attempt < 10) {
		// don't spawn too close to another star
		for (auto it = stars.begin(); it != stars.end(); it++) {
			if ((star->pos - (*it)->pos).length() <= SPAWN_DIST) {
				delete star;
				SpawnStar(attempt + 1);
				return;
			}
		}
	}

	stars.push_back(star);
}

void Game::Shutdown() {
}

void Game::WindowMove(vec2 newPos) {
	vec2 diff = newPos - GetWindowPos();

	if (ball->collider.CollideWithBorders(screen))
		ball->vel = diff * BALL_BOUNCE_SCALE;
}

void Game::DrawBackground() {
	int w = screen->GetWidth(), h = screen->GetHeight();
	for (int y = 0; y < h / BG_TILE_SIZE; y++) {
		for (int x = 0; x < w / BG_TILE_SIZE; x++)
			bgSprite.DrawScaled(
				x * BG_TILE_SIZE, y * BG_TILE_SIZE,
				BG_TILE_SIZE, BG_TILE_SIZE,
				screen
			);
	}
}

void Game::KeyDown(int key) {
	switch (state) {
		case STATE_STARTMENU: {
			if (key == SDL_SCANCODE_RETURN) Reset(STATE_GAME);
			if (key == SDL_SCANCODE_ESCAPE) state = STATE_EXIT;
			break;
		}
		case STATE_GAME: {
			if (key == SDL_SCANCODE_ESCAPE) state = STATE_STARTMENU;
			break;
		}
		case STATE_ENDMENU: {
			if (key == SDL_SCANCODE_ESCAPE) state = STATE_STARTMENU;
			
			// enter character
			if (key >= SDL_SCANCODE_A && key <= SDL_SCANCODE_Z && strlen(nameBuff) < NAME_LEN)
				nameBuff[strlen(nameBuff)] = key + ('a' - SDL_SCANCODE_A);

			// delete character
			if (key == SDL_SCANCODE_BACKSPACE && strlen(nameBuff))
				nameBuff[strlen(nameBuff) - 1] = 0;

			// confirm name
			if (key == SDL_SCANCODE_RETURN && strlen(nameBuff)) {
				saveScore(nameBuff, score);
				top3 = getTop3Scores();
				state = STATE_SCOREMENU;
			}

			break;
		}
		case STATE_SCOREMENU: {
			if (key == SDL_SCANCODE_RETURN) state = STATE_STARTMENU;
			break;
		}
		case STATE_EXIT:
			break;
	}
}

void Game::TickGame(float deltaTime) {
	DrawBackground();

	// spawn new star if needed
	if (time(NULL) - lastSpawnTime >= STAR_SPAWN_DELAY) {
		lastSpawnTime = time(NULL);
		if (stars.size() < MAX_STARS) SpawnStar();
	}

	// update player
	ball->Update(deltaTime);

	// update stars
	for (auto it = stars.begin(); it != stars.end();) {
		Star* star = *it;

		// check if the player has hit the star
		if (star->collider.CollidesWith(ball->collider)) {
			stars.erase(it);
			score++;
			continue;
		}

		// update the star
		star->Update(deltaTime);
		it++;
	}

	// update time left
	int secondsLeft = TOTAL_TIME - (time(NULL) - startTime);
	if (secondsLeft < 0) state = STATE_ENDMENU;

	// draw score and time left
	starSprite.DrawScaled(10, 10, 25, 25, screen);
	DrawText(screen, vec2(40, 2), 1, NONE, formatString("%d", score));
	DrawText(screen, vec2(10, 2), 1, RIGHT_ALIGN, formatString("%d", secondsLeft));

#ifdef _DEBUG
	// draw fps
	int fps = (1000 / deltaTime);
	DrawText(screen, vec2(10, 40), 1, RIGHT_ALIGN, formatString("%d fps", fps));
#endif
}

void Game::TickStartMenu() {
	// make background a little darker
	DrawBackground();
	screen->ScaleColor(19);

	// draw menu
	DrawText(screen, vec2(0, 80), 1.3, H_CENTER, "whack 'a");
	DrawText(screen, vec2(0, 120), 1.5, H_CENTER, "window!");
	starSprite.DrawScaled(100, 100, 60, 60, screen);
	ballSprite.DrawScaled(440, 100, 60, 60, screen);
	DrawText(screen, vec2(0, 250), 0.7, H_CENTER, "press enter to start");
	DrawText(screen, vec2(0, 280), 0.7, H_CENTER, "or escape to quit");
	DrawText(screen, vec2(5, 380), 0.4, RIGHT_ALIGN, "by sjoerd vermeulen");
}

void Game::TickEndMenu() {
	// make background darker
	DrawBackground();
	screen->ScaleColor(15);

	// draw menu
	DrawText(screen, vec2(0, 80), 1.5, H_CENTER, "game over!");
	DrawText(screen, vec2(0, 145), 0.8, H_CENTER, formatString("you collected %d stars", score));
	DrawText(screen, vec2(0, 220), 0.7, H_CENTER, "enter your name:");
	DrawText(screen, vec2(0, 250), 0.7, H_CENTER, formatString("%s_", nameBuff));
	DrawText(screen, vec2(0, 300), 0.6, H_CENTER, "or press escape to quit");
}

void Game::TickScoreMenu() {
	// make background darker
	DrawBackground();
	screen->ScaleColor(15);

	// draw scoreboard
	DrawText(screen, vec2(0, 60), 1.2, H_CENTER, "scoreboard:");
	for (int i = 0; i < top3.size(); i++) {
		char* name = formatString("%d. %s", i + 1, top3[i].name);
		char* score = formatString("%d", top3[i].score);
		DrawText(screen, vec2(142, 130 + 40 * i), 1, NONE, name);
		DrawText(screen, vec2(182, 130 + 40 * i), 1, RIGHT_ALIGN, score);
		starSprite.DrawScaled(432, 140 + 40 * i, 25, 25, screen);
	}
	DrawText(screen, vec2(0, 300), 0.6, H_CENTER, "press enter to continue");
}

void Game::Tick(float deltaTime) {
	switch (state) {
		case STATE_STARTMENU:
			TickStartMenu();
			break;
		case STATE_GAME:
			TickGame(deltaTime);
			break;
		case STATE_ENDMENU:
			TickEndMenu();
			break;
		case STATE_SCOREMENU:
			TickScoreMenu();
			break;
		case STATE_EXIT:
			break;
	}
}

};