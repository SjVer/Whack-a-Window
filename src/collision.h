#pragma once

#include "template.h"

namespace Tmpl8 {

typedef struct {
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;

    operator bool() { return top || bottom || left || right; } 
} Collision;

class CollisionCircle {
public:
    vec2* pos;
    float radius;
    
    CollisionCircle() {}
    CollisionCircle(vec2* pos_ptr, float radius):
        pos(pos_ptr), radius(radius) {}

    bool CollidesWith(CollisionCircle& coll) {
<<<<<<< HEAD
        vec2 realPos = *pos + vec2(radius, radius);
        vec2 otherRealPos = *coll.pos + vec2(coll.radius, coll.radius); 
=======
        vec2 realPos = *pos + vec2(radius, radius) * 2;
        vec2 otherRealPos = *coll.pos + vec2(coll.radius, coll.radius) * 2; 
>>>>>>> VisualStudio
        vec2 dist = realPos - otherRealPos;
        return dist.length() <= radius + coll.radius;
    }

    bool IsOutsideSurface(Surface* surface, bool global = false) {
        vec2 lpos = global ? *pos : *pos - GetWindowPos();

        return
            // left and right
            lpos.x < 0 ||
            lpos.x + 2 * radius > surface->GetWidth() ||
            // top and bottom
            lpos.y < 0 ||
            lpos.y + 2 * radius > surface->GetHeight()
        ;
    }

    Collision CollideWithBorders(Surface* screen) {
        vec2 lpos = *pos - GetWindowPos();
        
        return {
            // top and bottom
            lpos.y <= 0,
            lpos.y + 2 * radius >= screen->GetHeight(),
            // left and right
            lpos.x <= 0,
            lpos.x + 2 * radius >= screen->GetWidth(),
        };
    }
};

}