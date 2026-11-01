#pragma once
#include "raylib.h"

// Identificadores de sprites procedimentales 12x12
constexpr int ART_SLIME = 0;
constexpr int ART_BAT = 1;
constexpr int ART_HUMANOID = 2;
constexpr int ART_BEAST = 3;
constexpr int ART_DRAGON = 4;
constexpr int ART_HERO_BACK = 5;
constexpr int ART_WARRIOR = 6;
constexpr int ART_MAGE = 7;
constexpr int ART_SCOUT = 8;

void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX = false);
