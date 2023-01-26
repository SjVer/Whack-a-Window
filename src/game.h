#pragma once

#include "template.h"
#include "entities.h"
#include <vector>

namespace Tmpl8 {

class Surface;

class Game
{
public:
	Game(Surface* screen);
	void Reset(int newState);
	void Shutdown();
	void WindowMove( vec2 new_pos );
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( vec2 new_pos ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key );

	void SpawnStar(uint attempt);
	void DrawBackground();

	void TickStartMenu();
	void TickGame(float deltaTime);
	void TickEndMenu();
	void TickScoreMenu();

	vec2 mousePos;

	enum State {
		STATE_STARTMENU,
		STATE_GAME,
		STATE_ENDMENU,
		STATE_SCOREMENU,
		STATE_EXIT
	} state;
private:
	Surface* screen;

	time_t startTime;
	float timeSinceLastSpawn;

	std::vector<Star*> stars;
	Ball* ball;
	int score;
};

}; // namespace Tmpl8