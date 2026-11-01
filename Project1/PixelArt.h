#pragma once
#include "raylib.h"

// art: 0 blob, 1 murcielago, 2 humanoide, 3 bestia, 4 dragon, 5 heroe (de espaldas)
constexpr int ART_HERO_BACK = 5;

void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX = false);