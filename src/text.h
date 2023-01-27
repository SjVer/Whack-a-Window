#pragma once

#include "template.h"
#include "surface.h"
#include <cstring>

namespace Tmpl8 {

typedef enum : uchar {
    NONE        = 0b00000000,
    RIGHT_ALIGN = 0b00000001,
    H_CENTER    = 0b00000010,
    V_CENTER    = 0b00000100
} TextOptions;

char* formatString(const char *format, ...);

vec2 TextSize(float scale, char* text);

void DrawChar(Surface* surface, vec2 pos, float scale, TextOptions options, char chr);

void DrawText(Surface* surface, vec2 pos, float scale, TextOptions options, char* text);

}