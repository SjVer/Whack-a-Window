#include "surface.h"
#include "collision.h"

namespace Tmpl8 {

#define BALL_SCALE 0.5
#define BALL_ASSET "assets/RollingBall/ball_red_large.png"
<<<<<<< HEAD
#define MAX_VEL 5.f
#define DRAG_FACTOR 0.99
=======
>>>>>>> VisualStudio

#define STAR_SCALE 2
#define STAR_ASSET "assets/RollingBall/star.png"

static Sprite starSprite(new Surface(STAR_ASSET), 1);

class Ball {
	Sprite* sprite;
	Surface* surface;

public:
	vec2 pos, vel, size;
	CollisionCircle collider;

	Ball(Surface* s);
	void Update(float deltaTime);
};

class Star {
	Surface* surface;

public:
	vec2 pos, size;
	CollisionCircle collider;

	Star(Surface* s, vec2 p);
	void Update(float deltaTime);
};

}