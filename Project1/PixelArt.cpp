#include "PixelArt.h"
#include <cstring>

// Cada sprite: 12x12.  o=contorno  b=cuerpo  l=claro  w=blanco  r=rojo
// h=pelo/adorno  s=piel  d=azul/cuero  .=transparente
static const char* ART[12][12] = {
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
        "..ooo..ooo.." },
    { // 6 GUERRERO
        "...orrrro...",
        "..oobbbboo..",
        "..obssssbo..",
        "..obswwsbo..",
        "...osssso...",
        "..oobbbboo..",
        ".obollllobo.",
        ".obobllbobo.",
        "..oobbbboo..",
        "...oddood...",
        "...odd..dd..",
        "..ooo..ooo.." },
    { // 7 MAGA
        ".....oo.....",
        "....obbo....",
        "...obllbo...",
        "..oobbbboo..",
        "...osssso...",
        "..osswwsso..",
        "...osssso...",
        "..oobbbboo..",
        ".hoboblboboh",
        ".hobobbboboh",
        ".h.obbbbo..h",
        "...oooooo..." },
    { // 8 EXPLORADORA
        "....oooo....",
        "...obbbbo...",
        "..obhhhhbo..",
        "..obswwsbo..",
        "...osssso...",
        "...ohhhho...",
        "..ooddddoo..",
        ".loodllldoo.",
        ".loodddddool",
        "...ob..bo.l.",
        "...ob..bo...",
        "..ooo..ooo.." },
    { // 9 ANCIANO
        "....oooo....",
        "...ohhhho...",
        "..ohssssho..",
        "..ohswwsho..",
        "...owwwwo...",
        "...owwwwo...",
        "..oobbbboo..",
        ".obobllbobo.",
        ".obobllbobo.",
        "..oobbbboo..",
        "...ob..bo...",
        "..ooo..ooo.." },
    { // 10 CURANDERA
        "....oooo....",
        "...owwwwo...",
        "..owsssswo..",
        "..owswwswo..",
        "...osssso...",
        "..oobbbboo..",
        ".obollllobo.",
        ".obollllobo.",
        "..oobbbboo..",
        "...oddood...",
        "...odd..dd..",
        "..ooo..ooo.." },
    { // 11 CARTEL
        "............",
        "..oooooooo..",
        ".obbbbbbbbo.",
        ".obllllllbo.",
        ".obllllllbo.",
        ".obllllllbo.",
        ".obbbbbbbbo.",
        "..oooooooo..",
        "....obbo....",
        "....obbo....",
        "....obbo....",
        "...oooooo..." }
};

static Color Palette(char ch, Color body, Color light) {
    switch (ch) {
    case 'o': return { 25, 25, 40, 255 };
    case 'b': return body;
    case 'l': return light;
    case 'w': return WHITE;
    case 'r': return { 220, 50, 50, 255 };
    case 'h': return { 235, 180, 80, 255 };   // adorno dorado o pelo
    case 's': return { 225, 175, 135, 255 };  // piel
    case 'd': return { 45, 65, 110, 255 };    // ropaje secundario
    default:  return BLANK;
    }
}

void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX) {
    if (art < 0 || art > 11) return;
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
