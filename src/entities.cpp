#include "entities.h"
#include "sound.h"

namespace Tmpl8 {

#define MAX_VEL 3.f
#define DRAG_AMOUNT 0.001f

Ball::Ball(Surface* s) {
    surface = s;
    sprite = new Sprite(new Surface(BALL_ASSET), 1);
    size = vec2(sprite->GetWidth() * BALL_SCALE, sprite->GetHeight() * BALL_SCALE);

    vel = vec2(0, 0);
    pos = GetWindowPos();
    collider = CollisionCircle(&pos, size.x / 2.);

    // center it
    pos = GetMonitorSize() * 0.5;
}

void Ball::Update(float deltaTime) {
    vel *= Max(1. - DRAG_AMOUNT * deltaTime, 0.);
    vel.x = Min(vel.x, MAX_VEL);
    vel.y = Min(vel.y, MAX_VEL);

    pos += vel * deltaTime;
    vec2 wpos = GetWindowPos();

    // clamp to collided window border and bounce
    if (Collision coll = collider.CollideWithBorders(surface)) {
        //PlaySingleSound(SOUND_BOUNCE);

        if (coll.left || coll.right) {
            pos.x = coll.left ? wpos.x : wpos.x + surface->GetWidth() - size.x; 
            vel.x *= -1;
        }
        if (coll.top || coll.bottom) {
            pos.y = coll.top ? wpos.y : wpos.y + surface->GetHeight() - size.y; 
            vel.y *= -1;
        }
    }
    
    vec2 lpos = pos - wpos;
    sprite->DrawScaled(lpos.x, lpos.y, size.x, size.y, surface);
}

Star::Star(Surface* s, vec2 p) {
    vec2 winpos = GetWindowPos();

    surface = s;
    pos = p + winpos;

    size = vec2(starSprite.GetWidth() * STAR_SCALE, starSprite.GetHeight() * STAR_SCALE);
    collider = CollisionCircle(&pos, starSprite.GetWidth() / 2.);
}

void Star::Update(float deltaTime) {
    if (!collider.IsOutsideSurface(surface)) {
        vec2 lpos = pos - GetWindowPos();
        starSprite.DrawScaled(lpos.x, lpos.y, size.x, size.y, surface);
    }
}

}