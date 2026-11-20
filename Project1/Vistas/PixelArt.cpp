/**
 * @file PixelArt.cpp
 * @brief Implementación de las plantillas matriciales 12x12 y la paleta de dibujo procedural.
 *
 * Capa de Arquitectura: Vista (View) - Renderizado Procedural.
 *
 * Propósito y Responsabilidades:
 * - Definir en memoria estática las 12 matrices ASCII de 12x12 caracteres.
 * - Mapear cada carácter clave a una estructura `Color` de Raylib.
 * - Rasterizar cada celda del sprite aplicando escala y volteo horizontal si se requiere.
 */

#include "PixelArt.h"
#include <cstring>

/**
 * @brief Matrices de 12x12 caracteres que codifican el aspecto gráfico de cada sprite.
 *
 * Leyenda de caracteres:
 * - '.' : Píxel transparente (se ignora durante el dibujado).
 * - 'o' : Contorno oscuro ({ 25, 25, 40, 255 }).
 * - 'b' : Color primario del cuerpo (`body`).
 * - 'l' : Color secundario o brillo (`light`).
 * - 'w' : Blanco puro para esclerótica o toques de luz (WHITE).
 * - 'r' : Rojo intenso para ojos hostiles, bocas o gemas.
 * - 'h' : Dorado o cabello ({ 235, 180, 80, 255 }).
 * - 's' : Tono de piel cálido ({ 225, 175, 135, 255 }).
 * - 'd' : Cuero oscuro o ropaje secundario ({ 45, 65, 110, 255 }).
 */
static const char* ART[12][12] = {
    { // 0 BLOB (Slime gelatinoso)
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
    { // 1 MURCIELAGO (Alas de membranas)
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
    { // 2 HUMANOIDE (Goblin / Esqueleto bípedo)
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
    { // 3 BESTIA (Lobo cuadrúpedo con orejas alertas)
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
    { // 4 DRAGON (Jefe final con cuernos, fauces y vientre de escamas)
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
    { // 5 HEROE DE ESPALDAS (Perspectiva para combate)
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
    { // 6 GUERRERO (Yelmo con pluma roja y cota metálica)
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
    { // 7 MAGA (Capucha arcana y mechones mágicos)
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
    { // 8 EXPLORADORA (Gorra de cazador con pluma y carcaj)
        "....oooo....",
        "...obbbbo...",
        "..obhhhhbo..",
        "..obswwsbo..",
        "...osssso...",
        "..ohhhho...",
        "..ooddddoo..",
        ".loodllldoo.",
        ".loodddddool",
        "...ob..bo.l.",
        "...ob..bo...",
        "..ooo..ooo.." },
    { // 9 ANCIANO (Mentor Aldor con barba blanca y cejas sabias)
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
    { // 10 CURANDERA (Elena con cofia blanca y hábito sanador)
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
    { // 11 CARTEL (Tablón de madera con soporte inferior)
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

/**
 * @brief Resuelve el color final de un píxel a partir de su clave de carácter y la paleta personalizada.
 *
 * @param ch Carácter del sprite ('o', 'b', 'l', 'w', 'r', 'h', 's', 'd').
 * @param body Color dinámico asignado al cuerpo.
 * @param light Color dinámico asignado a ribetes o brillos.
 * @return Color Color RGBA resultante para pintar en el framebuffer.
 */
static Color Palette(char ch, Color body, Color light) {
    switch (ch) {
    case 'o': return { 25, 25, 40, 255 };
    case 'b': return body;
    case 'l': return light;
    case 'w': return WHITE;
    case 'r': return { 220, 50, 50, 255 };
    case 'h': return { 235, 180, 80, 255 };   // Adorno dorado o pelo
    case 's': return { 225, 175, 135, 255 };  // Tono de piel
    case 'd': return { 45, 65, 110, 255 };    // Ropaje secundario o cuero
    default:  return BLANK;
    }
}

/**
 * @brief Dibuja el sprite procedimental celda a celda en pantalla.
 *
 * Itera las 12 filas y 12 columnas. Si el carácter es distinto del punto de transparencia ('.'),
 * calcula la posición en pantalla considerando el factor de escala y el volteo en X (`flipX`).
 */
void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX) {
    if (art < 0 || art > 11) return;

    for (int r = 0; r < 12; ++r) {
        const char* row = ART[art][r];
        int len = static_cast<int>(std::strlen(row));
        for (int c = 0; c < len && c < 12; ++c) {
            if (row[c] == '.') continue;
            // Si flipX es true, invierte el índice de columna (0 pasa a 11, etc.)
            int cx = flipX ? (11 - c) : c;
            DrawRectangle(
                static_cast<int>(x) + cx * scale,
                static_cast<int>(y) + r * scale,
                scale,
                scale,
                Palette(row[c], body, light)
            );
        }
    }
}
