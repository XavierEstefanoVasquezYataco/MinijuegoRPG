#include "MapView.h"
#include "PixelArt.h"

#include <cmath>

static const char* PAUSE_LABELS[3] = {
    "Reanudar",
    "Reiniciar",
    "Salir"
};

void MapView::Load() {
    // Sprite opcional: assets/hero.png
    // Se espera una hoja de 4 columnas x 4 filas, con frames de 32x32.
    if (FileExists("assets/hero.png")) {
        heroSheet = LoadTexture("assets/hero.png");
        hasSheet = heroSheet.id > 0;
    }
}

void MapView::Unload() {
    if (hasSheet) {
        UnloadTexture(heroSheet);
        heroSheet = {};
        hasSheet = false;
    }
}

void MapView::DrawTile(const TileMap& map, int col, int row) const {
    const int tileSize = TileMap::TILE;
    const int x = col * tileSize;
    const int y = row * tileSize;

    char tile = map.At(col, row);

    // Base de pasto
    DrawRectangle(x, y, tileSize, tileSize, { 88, 168, 96, 255 });

    switch (tile) {
    case '#':
        // Árboles representados con primitivas de Raylib
        DrawCircle(x + 16, y + 16, 17, { 30, 100, 50, 255 });
        DrawCircle(x + 12, y + 12, 8, { 55, 135, 75, 255 });
        DrawCircle(x + 22, y + 18, 7, { 45, 120, 60, 255 });
        break;

    case 'p':
        // Camino
        DrawRectangle(x, y, tileSize, tileSize, { 232, 208, 152, 255 });
        break;

    case 'W':
        // Agua
        DrawRectangle(x, y, tileSize, tileSize, { 110, 125, 165, 255 });
        DrawRectangle(x + 4, y + 8, 10, 2, { 160, 190, 220, 255 });
        break;

    case 'H':
        // Casa sencilla formada con primitivas
        DrawRectangle(x + 4, y + 10, 24, 22, { 215, 215, 225, 255 });

        if (map.At(col, row - 1) != 'H') {
            DrawTriangle(
                { static_cast<float>(x + 2), static_cast<float>(y + 12) },
                { static_cast<float>(x + 16), static_cast<float>(y - 2) },
                { static_cast<float>(x + 30), static_cast<float>(y + 12) },
                { 215, 85, 55, 255 }
            );
        }

        DrawRectangle(x + 13, y + 20, 7, 12, { 90, 130, 190, 255 });
        break;

    case '1':
    case '2':
        // Compuerta lógica cerrada
        DrawRectangle(x, y, tileSize, tileSize, { 232, 208, 152, 255 });
        DrawRectangle(x + 4, y + 2, 6, tileSize - 4, { 110, 70, 40, 255 });
        DrawRectangle(x + 22, y + 2, 6, tileSize - 4, { 110, 70, 40, 255 });
        DrawRectangle(x + 2, y + 8, tileSize - 4, 5, { 140, 90, 50, 255 });
        DrawRectangle(x + 2, y + 20, tileSize - 4, 5, { 140, 90, 50, 255 });
        break;

    default:
        // Pasto, puntos de aparición ya extraídos u otros tiles
        break;
    }

    // Borde tenue para distinguir las celdas
    DrawRectangleLines(x, y, tileSize, tileSize, Fade(DARKGREEN, 0.12f));
}

void MapView::DrawPlayer(const Player& player) const {
    const float centerX = player.body.x + player.body.width / 2.0f;
    const float bottomY = player.body.y + player.body.height;

    DrawEllipse(
        static_cast<int>(centerX),
        static_cast<int>(bottomY),
        11,
        4,
        Fade(BLACK, 0.30f)
    );

    if (hasSheet) {
        int frame = 0;

        if (player.moving) {
            frame = static_cast<int>(player.animTimer * 8.0f) % 4;
        }

        Rectangle source{
            frame * 32.0f,
            static_cast<float>(static_cast<int>(player.facing) * 32),
            32.0f,
            32.0f
        };

        Rectangle destination{
            centerX - 16.0f,
            bottomY - 30.0f,
            32.0f,
            32.0f
        };

        DrawTexturePro(
            heroSheet,
            source,
            destination,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        return;
    }

    // Personaje dibujado con primitivas si no existe assets/hero.png
    const float bob = player.moving
        ? std::sin(player.animTimer * 16.0f) * 2.0f
        : 0.0f;

    const float legMove = player.moving
        ? std::sin(player.animTimer * 16.0f) * 3.0f
        : 0.0f;

    // Piernas
    DrawRectangle(
        static_cast<int>(centerX - 7 + legMove),
        static_cast<int>(bottomY - 8),
        5,
        8,
        DARKBLUE
    );

    DrawRectangle(
        static_cast<int>(centerX + 2 - legMove),
        static_cast<int>(bottomY - 8),
        5,
        8,
        DARKBLUE
    );

    // Cuerpo
    DrawRectangle(
        static_cast<int>(centerX - 8),
        static_cast<int>(bottomY - 20 + bob),
        16,
        13,
        SKYBLUE
    );

    // Cabeza y pelo
    DrawCircle(
        static_cast<int>(centerX),
        static_cast<int>(bottomY - 25 + bob),
        7,
        { 205, 150, 110, 255 }
    );

    DrawRectangle(
        static_cast<int>(centerX - 7),
        static_cast<int>(bottomY - 34 + bob),
        14,
        6,
        { 40, 200, 190, 255 }
    );

    // Ojos orientados según la dirección
    const int eyeOffsetX =
        player.facing == Facing::LEFT ? -2 :
        player.facing == Facing::RIGHT ? 2 : 0;

    if (player.facing != Facing::UP) {
        DrawCircle(
            static_cast<int>(centerX - 3 + eyeOffsetX),
            static_cast<int>(bottomY - 25 + bob),
            1,
            BLACK
        );

        DrawCircle(
            static_cast<int>(centerX + 3 + eyeOffsetX),
            static_cast<int>(bottomY - 25 + bob),
            1,
            BLACK
        );
    }
}

void MapView::DrawEnemy(const Enemy& enemy) const {
    const float centerX = enemy.body.x + enemy.body.width / 2.0f;
    const float centerY = enemy.body.y + enemy.body.height / 2.0f;

    const int scale = enemy.isBoss ? 3 : 2;
    const float spriteSize = 12.0f * scale;

    DrawEllipse(
        static_cast<int>(centerX),
        static_cast<int>(enemy.body.y + enemy.body.height),
        static_cast<int>(spriteSize / 2.0f),
        4,
        Fade(BLACK, 0.30f)
    );

    DrawPixelArt(
        enemy.art,
        centerX - spriteSize / 2.0f,
        centerY - spriteSize / 2.0f - 4.0f,
        scale,
        enemy.bodyColor,
        enemy.lightColor
    );

    if (enemy.ai == AIState::CHASE) {
        DrawText(
            "!",
            static_cast<int>(centerX) - 3,
            static_cast<int>(centerY - spriteSize / 2.0f - 22.0f),
            20,
            RED
        );
    }

    DrawText(
        enemy.name.c_str(),
        static_cast<int>(centerX) - MeasureText(enemy.name.c_str(), 10) / 2,
        static_cast<int>(centerY - spriteSize / 2.0f - 12.0f),
        10,
        WHITE
    );
}

void MapView::DrawWorld(
    const TileMap& map,
    const Player& player,
    const std::vector<Enemy>& enemies
) const {
    ClearBackground(BLACK);

    for (int row = 0; row < TileMap::ROWS; ++row) {
        for (int col = 0; col < TileMap::COLS; ++col) {
            DrawTile(map, col, row);
        }
    }

    for (const Enemy& enemy : enemies) {
        if (!enemy.defeated) {
            DrawEnemy(enemy);
        }
    }

    DrawPlayer(player);
}

void MapView::DrawHUD(const Player& player) const {
    DrawRectangle(0, 512, 960, 28, { 20, 20, 30, 255 });

    DrawText(
        TextFormat(
            "Nv.%d  HP %d/%d  MP %d/%d  Pociones %d",
            player.level,
            player.hp,
            player.maxHp,
            player.mp,
            player.maxMp,
            player.potions
        ),
        10,
        518,
        16,
        WHITE
    );

    DrawText(
        "WASD: mover | P/ESC: pausa | M: minimapa",
        560,
        520,
        14,
        LIGHTGRAY
    );
}

void MapView::DrawMinimap(
    const TileMap& map,
    const Player& player,
    const std::vector<Enemy>& enemies
) const {
    constexpr int scale = 4;

    const int mapWidth = TileMap::COLS * scale;
    const int mapHeight = TileMap::ROWS * scale;
    const int originX = 960 - mapWidth - 8;
    const int originY = 8;

    DrawRectangle(
        originX - 3,
        originY - 3,
        mapWidth + 6,
        mapHeight + 6,
        Fade(BLACK, 0.80f)
    );

    for (int row = 0; row < TileMap::ROWS; ++row) {
        for (int col = 0; col < TileMap::COLS; ++col) {
            const char tile = map.At(col, row);

            Color color = { 120, 200, 110, 255 };

            if (tile == '#') {
                color = { 30, 100, 50, 255 };
            }
            else if (tile == 'p') {
                color = { 240, 230, 180, 255 };
            }
            else if (tile == 'W') {
                color = { 70, 100, 200, 255 };
            }
            else if (tile == 'H') {
                color = { 200, 120, 90, 255 };
            }
            else if (tile == '1' || tile == '2') {
                color = { 130, 80, 40, 255 };
            }

            DrawRectangle(
                originX + col * scale,
                originY + row * scale,
                scale,
                scale,
                color
            );
        }
    }

    for (const Enemy& enemy : enemies) {
        if (!enemy.defeated) {
            const Vector2 center = enemy.Center();

            const int miniX =
                originX + static_cast<int>(center.x / TileMap::TILE * scale);

            const int miniY =
                originY + static_cast<int>(center.y / TileMap::TILE * scale);

            DrawRectangle(miniX - 1, miniY - 1, 4, 4, RED);
        }
    }

    const Vector2 playerCenter = player.Center();

    const int playerX =
        originX + static_cast<int>(playerCenter.x / TileMap::TILE * scale);

    const int playerY =
        originY + static_cast<int>(playerCenter.y / TileMap::TILE * scale);

    DrawRectangle(playerX - 2, playerY - 2, 5, 5, WHITE);
}

Rectangle MapView::PauseButton(int index) const {
    return {
        350.0f,
        230.0f + index * 65.0f,
        260.0f,
        50.0f
    };
}

int MapView::GetPauseButton(Vector2 mouse) const {
    for (int i = 0; i < 3; ++i) {
        if (CheckCollisionPointRec(mouse, PauseButton(i))) {
            return i;
        }
    }

    return -1;
}

void MapView::DrawPause(int selected) const {
    DrawRectangle(0, 0, 960, 540, Fade(BLACK, 0.65f));

    const char* title = "PAUSA";
    const int titleWidth = MeasureText(title, 50);

    DrawText(
        title,
        (960 - titleWidth) / 2,
        150,
        50,
        GOLD
    );

    for (int i = 0; i < 3; ++i) {
        const Rectangle button = PauseButton(i);
        const bool isSelected = i == selected;

        DrawRectangleRec(
            button,
            isSelected ? GOLD : DARKBLUE
        );

        DrawRectangleLinesEx(
            button,
            3,
            RAYWHITE
        );

        const int labelWidth = MeasureText(PAUSE_LABELS[i], 24);

        DrawText(
            PAUSE_LABELS[i],
            static_cast<int>(button.x + (button.width - labelWidth) / 2),
            static_cast<int>(button.y + 13),
            24,
            isSelected ? BLACK : WHITE
        );
    }

    DrawText(
        "Flechas + ENTER, o mouse. P/ESC para reanudar.",
        300,
        450,
        16,
        LIGHTGRAY
    );
}