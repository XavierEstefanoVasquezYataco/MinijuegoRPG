#include "PixelArt.h"
#include <cstring>

// Cada sprite: 12x12.  o=contorno  b=cuerpo  l=claro  w=blanco  r=rojo
// h=pelo  s=piel  d=azul oscuro  .=transparente
static const char* ART[6][12] = {
    { // 0 BLOB
        "............",
        "............",
        "....oooo....",
        "...obbbbo...",
        "..obllbbbo..",
        "..oblbbbbo..",
        ".obbbbbbbbo.",
        ".obwobbowbo.",
        ".obbbbbbbbo.",
        ".obbbrrbbbo.",
        "..oobbbboo..",
        "....oooo...." },
    { // 1 MURCIELAGO
        "............",
        "o....oo....o",
        "oo..obbo..oo",
        "obooobbooobo",
        "obbobwbwbobo",
        ".obbbbbbbbo.",
        "..obbrrbbo..",
        "...obbbbo...",
        "....oooo....",
        "............",
        "............",
        "............" },
    { // 2 HUMANOIDE
        "....oooo....",
        "...obbbbo...",
        "...obwbwo...",
        "...obbbbo...",
        "....orro....",
        "..oobbbboo..",
        ".obobllbobo.",
        ".obobllbobo.",
        "..oobbbboo..",
        "...obbbbo...",
        "...ob..bo...",
        "..ooo..ooo.." },
    { // 3 BESTIA
        "............",
        "..o.o.......",
        ".obobo....o.",
        ".obbbboooobo",
        "obwbbbbbbbbo",
        "orbbbbbbbbbo",
        ".obbbbbbbbbo",
        "..obbbbbbbo.",
        "..obobbbobo.",
        "..obo.obobo.",
        "..obo..obo..",
        "..ooo..ooo.." },
    { // 4 DRAGON
        "...o....o...",
        "..oro..oro..",
        ".obbbooobbo.",
        "obbwbbbbwbbo",
        "obbbbbbbbbbo",
        "obbrrrrrrbbo",
        ".obbbbbbbbo.",
        ".oblllllllbo",
        "obblllllllbb",
        ".obbllllbbo.",
        "..obbo.obbo.",
        "..ooo..ooo.." },
    { // 5 HEROE DE ESPALDAS
        "...oooooo...",
        "..ohhhhhho..",
        "..ohhhhhho..",
        "..ohhhhhho..",
        "...osssso...",
        "..obbbbbbo..",
        ".obbbbbbbbo.",
        ".osbbbbbbso.",
        "..obbbbbbo..",
        "..oddooddo..",
        "..odd..ddo..",
        "..ooo..ooo.." }
};

static Color Palette(char ch, Color body, Color light) {
    switch (ch) {
    case 'o': return { 25, 25, 40, 255 };
    case 'b': return body;
    case 'l': return light;
    case 'w': return WHITE;
    case 'r': return { 220, 50, 50, 255 };
    case 'h': return { 40, 200, 190, 255 };   // pelo turquesa
    case 's': return { 220, 165, 125, 255 };
    case 'd': return { 40, 60, 130, 255 };
    default:  return BLANK;
    }
}

void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX) {
    if (art < 0 || art > 5) return;
    for (int r = 0; r < 12; ++r) {
        const char* row = ART[art][r];
        int len = static_cast<int>(std::strlen(row));
        for (int c = 0; c < len && c < 12; ++c) {
            if (row[c] == '.') continue;
            int cx = flipX ? (11 - c) : c;
            DrawRectangle(static_cast<int>(x) + cx * scale,
                static_cast<int>(y) + r * scale,
                scale, scale, Palette(row[c], body, light));
        }
    }
}