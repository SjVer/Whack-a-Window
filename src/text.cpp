#include "text.h"
#include <cstdarg>
#include <cassert>

namespace Tmpl8 {

static Sprite* charSprites[255] = { NULL };

#define CHARS "abcdefghijklmnopqrstuvwxyz1234567890?!',.:-/_"
#define FILE_FMT "assets/converted_font/out/char_%d.png"
#define WHITESPACE_WIDTH 16

bool InitFont() {
    for (int i = 0; i < strlen(CHARS); i++) {
        char file[64];
        snprintf(file, 64, FILE_FMT, CHARS[i]);

        charSprites[(int)CHARS[i]] = new Sprite(new Surface(file), 1);
    }

    printf("font initialized (%zu glyphs).\n", strlen(CHARS));
    return true;
}

// init font automatically
bool _discard = InitFont();

char* formatString(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int smallSize = sizeof(char) * 1024;
    char* smallBuffer = (char*)malloc(smallSize);
    
    int size = vsnprintf(smallBuffer, smallSize, format, args);

    va_end(args);
    
    if (size < sizeof smallBuffer)
        return smallBuffer;

    int bigSize = sizeof(char) * (size + 1);
    char* buffer = (char*)malloc(bigSize);

    va_start(args, format);
    vsnprintf(buffer, bigSize, format, args);
    va_end(args);
    
    return buffer;
}

inline Sprite* getCharSprite(char chr) {
    Sprite* sprite = charSprites[(int)chr];
    
    if (!sprite) {
        fprintf(stderr, "Invalid character '%c' (%d)!\n", chr, chr);
        exit(1);
    }

    return sprite;
}

vec2 TextSize(float scale, char* text) {
    vec2 size(0, 0);
    
    for (int i = 0; i < strlen(text); i++) {
        if (text[i] == ' ') {
            size.x += WHITESPACE_WIDTH * scale;
            continue;
        }

        Sprite* sprite = getCharSprite(text[i]);
        size.x += sprite->GetWidth() * scale;
        size.y = Max(size.y, sprite->GetHeight() * scale);
    }

    return size;
}

void DrawChar(Surface* surface, vec2 pos, float scale, char chr) {
    Sprite* sprite = getCharSprite(chr);

    int height = sprite->GetHeight() * scale;
    int width = sprite->GetWidth() * scale;

    sprite->DrawScaled(pos.x, pos.y, width, height, surface);
}

void DrawText(Surface* surface, vec2 pos, float scale, TextOptions options, char* text) {
    // keep track of printing position
    vec2 currpos(pos);

    vec2 size = TextSize(scale, text);
    if (options & H_CENTER) currpos.x = (surface->GetWidth() - size.x) / 2;
    if (options & V_CENTER) currpos.y = (surface->GetHeight() - size.y) / 2;
    if (options & RIGHT_ALIGN) currpos.x = surface->GetWidth() - size.x - pos.x;

    for (int i = 0; i < strlen(text); i++) {
        if (text[i] == ' ') {
            currpos.x += WHITESPACE_WIDTH * scale;
            continue;
        }
        DrawChar(surface, currpos, scale, text[i]);
        currpos.x += charSprites[(int)text[i]]->GetWidth() * scale;
    }
}

}