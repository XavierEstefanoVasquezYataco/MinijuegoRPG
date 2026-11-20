/**
 * @file MapView.cpp
 * @brief Implementación del renderizado del supramundo, frustum culling, HUD y minimapa.
 *
 * Capa de Arquitectura: Vista (View) - Renderizado de Exploración.
 *
 * Propósito y Responsabilidades:
 * - Implementar el renderizado optimizado de los 4 sectores del mundo (Aldea, Pradera, Bosque, Ruinas).
 * - Realizar culling de celdas para dibujar únicamente las casillas visibles dentro del frustum de la cámara.
 * - Renderizar al héroe mediante hoja de sprites (`assets/hero.png`) o mediante dibujo vectorial procedural de respaldo.
 * - Dibujar la barra de estado (HUD), el minimapa proporcional con visor de cámara y la ventana de pausa.
 */

#include "MapView.h"
#include "PixelArt.h"
#include <algorithm>
#include <cmath>

/// Opciones textuales del menú de pausa.
static const char* PAUSE_LABELS[3] = {
    "Reanudar",
    "Reiniciar",
    "Volver al Titulo"
};

/**
 * @brief Carga la hoja de sprites del héroe si el archivo `assets/hero.png` existe en disco.
 *
 * La hoja esperada tiene formato de 4 columnas (cuadros de animación) x 4 filas (orientaciones),
 * cada celda de 32x32 píxeles. Si el archivo no está presente, activa un mecanismo de respaldo
 * seguro basado en dibujo procedural con primitivas de Raylib sin bloquear la ejecución.
 */
void MapView::Load() {
    if (FileExists("assets/hero.png")) {
        heroSheet = LoadTexture("assets/hero.png");
        hasSheet = heroSheet.id > 0;
    }
}

/**
 * @brief Libera la textura del héroe de la memoria de vídeo si fue cargada previamente.
 */
void MapView::Unload() {
    if (hasSheet) {
        UnloadTexture(heroSheet);
        heroSheet = {};
        hasSheet = false;
    }
}

/**
 * @brief Dibuja una celda individual del mapa decorándola según el sector al que pertenece.
 *
 * Reglas de tematización por sector:
 * - Sector 0 (cols 0-16): Aldea Segura. Césped cuidado, caminos de adoquín claro y casas con techos de tejas rojas.
 * - Sector 1 (cols 17-28): Pradera Abierta. Suelo verde claro con flores silvestres deterministas (sin RNG en runtime).
 * - Sector 2 (cols 29-39): Bosque Sombrío. Terreno oscuro, follaje de copas densas y agua nocturna.
 * - Sector 3 (cols 40-59): Ruinas Antiguas. Losas de piedra grisácea con grietas, pilares y muros en ruinas.
 *
 * Casillas especiales:
 * - Compuertas '1' y '2': Dibujadas como maderas reforzadas con remaches dorados mientras permanecen cerradas.
 */
void MapView::DrawTile(const TileMap& map, int col, int row) const {
    const int tileSize = TileMap::TILE;
    const int x = col * tileSize;
    const int y = row * tileSize;

    const char tile = map.At(col, row);

    // Identificación de sector según la columna
    const int sector = (col < 17) ? 0 : (col < 29) ? 1 : (col < 40) ? 2 : 3;

    // Paleta base del suelo según el sector
    Color groundColor;
    switch (sector) {
    case 0: groundColor = { 92, 172, 98, 255 }; break;   // Aldea: césped cuidado
    case 1: groundColor = { 108, 188, 92, 255 }; break;  // Pradera: verde claro
    case 2: groundColor = { 50, 110, 60, 255 }; break;   // Bosque: verde oscuro sombrío
    case 3:
    default: groundColor = { 110, 115, 122, 255 }; break; // Ruinas: losa de piedra gris
    }

    DrawRectangle(x, y, tileSize, tileSize, groundColor);

    // Detalles decorativos deterministas según el sector (hash matemático basado en coordenadas)
    const int detHash = (col * 31 + row * 17) % 23;
    if (tile == '.' || tile == 'p') {
        if (sector == 1) { // Pradera: flores discretas
            if (detHash == 1) {
                DrawRectangle(x + 10, y + 14, 3, 3, { 250, 250, 245, 255 }); // Flor blanca
            }
            else if (detHash == 5) {
                DrawRectangle(x + 22, y + 20, 3, 3, { 255, 225, 80, 255 }); // Flor amarilla
            }
        }
        else if (sector == 3) { // Ruinas: fisuras sutiles en la piedra
            if (detHash == 2) {
                DrawLine(x + 6, y + 8, x + 16, y + 14, { 80, 85, 92, 255 });
            }
        }
    }

    switch (tile) {
    case '#':
        if (sector == 3) {
            // Pilares de piedra derruidos en ruinas
            DrawRectangle(x + 4, y + 4, 24, 24, { 85, 90, 100, 255 });
            DrawRectangleLines(x + 4, y + 4, 24, 24, { 60, 65, 75, 255 });
            DrawRectangle(x + 8, y + 8, 16, 16, { 115, 120, 130, 255 });
        }
        else {
            // Árboles con tono según bosque o pradera
            Color treeBase = (sector == 2) ? Color{ 18, 65, 32, 255 } : Color{ 30, 100, 50, 255 };
            Color treeMid = (sector == 2) ? Color{ 30, 90, 45, 255 } : Color{ 55, 135, 75, 255 };
            Color treeHigh = (sector == 2) ? Color{ 25, 80, 40, 255 } : Color{ 45, 120, 60, 255 };
            DrawCircle(x + 16, y + 16, 17, treeBase);
            DrawCircle(x + 12, y + 12, 8, treeMid);
            DrawCircle(x + 22, y + 18, 7, treeHigh);
        }
        break;

    case 'p':
        // Caminos adaptados al bioma
        if (sector == 0) {
            DrawRectangle(x, y, tileSize, tileSize, { 236, 216, 168, 255 }); // Adoquín claro
        }
        else if (sector == 1) {
            DrawRectangle(x, y, tileSize, tileSize, { 222, 202, 145, 255 }); // Tierra clara
        }
        else if (sector == 2) {
            DrawRectangle(x, y, tileSize, tileSize, { 165, 135, 95, 255 });  // Sendero humus oscuro
        }
        else {
            DrawRectangle(x, y, tileSize, tileSize, { 145, 150, 158, 255 }); // Baldosas antiguas
            DrawRectangleLines(x + 2, y + 2, tileSize - 4, tileSize - 4, { 120, 125, 135, 255 });
        }
        break;

    case 'W':
        // Cuerpos de agua
        if (sector == 2 || sector == 3) {
            DrawRectangle(x, y, tileSize, tileSize, { 65, 85, 125, 255 });
            DrawRectangle(x + 4, y + 8, 10, 2, { 110, 135, 175, 255 });
        }
        else {
            DrawRectangle(x, y, tileSize, tileSize, { 110, 125, 165, 255 });
            DrawRectangle(x + 4, y + 8, 10, 2, { 160, 190, 220, 255 });
        }
        break;

    case 'H':
        // Casas y muros de piedra
        if (sector == 3) {
            // Muros y cubiles en ruinas
            DrawRectangle(x + 3, y + 6, 26, 24, { 95, 100, 108, 255 });
            DrawRectangleLines(x + 3, y + 6, 26, 24, { 60, 65, 72, 255 });
            DrawRectangle(x + 10, y + 14, 12, 16, { 50, 55, 62, 255 });
        }
        else {
            DrawRectangle(x + 4, y + 10, 24, 22, { 225, 220, 225, 255 });
            if (map.At(col, row - 1) != 'H') {
                DrawTriangle(
                    { static_cast<float>(x + 2), static_cast<float>(y + 12) },
                    { static_cast<float>(x + 16), static_cast<float>(y - 2) },
                    { static_cast<float>(x + 30), static_cast<float>(y + 12) },
                    { 215, 80, 55, 255 }
                );
            }
            DrawRectangle(x + 13, y + 20, 7, 12, { 90, 130, 190, 255 });
        }
        break;

    case '1':
    case '2':
        // Compuerta lógica cerrada con remaches
        DrawRectangle(x, y, tileSize, tileSize, { 210, 190, 140, 255 });
        DrawRectangle(x + 4, y + 2, 6, tileSize - 4, { 110, 70, 40, 255 });
        DrawRectangle(x + 22, y + 2, 6, tileSize - 4, { 110, 70, 40, 255 });
        DrawRectangle(x + 2, y + 8, tileSize - 4, 5, { 150, 95, 50, 255 });
        DrawRectangle(x + 2, y + 20, tileSize - 4, 5, { 150, 95, 50, 255 });
        // Remaches
        DrawRectangle(x + 6, y + 10, 2, 2, GOLD);
        DrawRectangle(x + 24, y + 10, 2, 2, GOLD);
        DrawRectangle(x + 6, y + 22, 2, 2, GOLD);
        DrawRectangle(x + 24, y + 22, 2, 2, GOLD);
        break;

    default:
        break;
    }

    // Borde tenue de celda
    DrawRectangleLines(x, y, tileSize, tileSize, Fade(BLACK, 0.08f));
}

/**
 * @brief Dibuja al personaje del jugador con sombra en el suelo y animación de pasos.
 */
void MapView::DrawPlayer(const Player& player) const {
    const float centerX = player.body.x + player.body.width / 2.0f;
    const float bottomY = player.body.y + player.body.height;

    // Sombra elíptica bajo los pies
    DrawEllipse(
        static_cast<int>(centerX),
        static_cast<int>(bottomY),
        11.0f,
        4.0f,
        Fade(BLACK, 0.30f)
    );

    // Caso 1: Si existe la hoja de sprites assets/hero.png
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

    // Caso 2: Respaldo procedimental con primitivas vectoriales de Raylib
    const float bob = player.moving
        ? std::sin(player.animTimer * 16.0f) * 2.0f
        : 0.0f;

    const float legMove = player.moving
        ? std::sin(player.animTimer * 16.0f) * 3.0f
        : 0.0f;

    // Piernas
    DrawRectangle(static_cast<int>(centerX - 7 + legMove), static_cast<int>(bottomY - 8), 5, 8, DARKBLUE);
    DrawRectangle(static_cast<int>(centerX + 2 - legMove), static_cast<int>(bottomY - 8), 5, 8, DARKBLUE);

    // Torso / Ropa
    DrawRectangle(static_cast<int>(centerX - 8), static_cast<int>(bottomY - 20 + bob), 16, 13, SKYBLUE);

    // Cabeza y pelo
    DrawCircle(static_cast<int>(centerX), static_cast<int>(bottomY - 25 + bob), 7, { 205, 150, 110, 255 });
    DrawRectangle(static_cast<int>(centerX - 7), static_cast<int>(bottomY - 34 + bob), 14, 6, { 40, 200, 190, 255 });

    // Ojos orientados según Facing
    const int eyeOffsetX =
        player.facing == Facing::LEFT ? -2 :
        player.facing == Facing::RIGHT ? 2 : 0;

    if (player.facing != Facing::UP) {
        DrawCircle(static_cast<int>(centerX - 3 + eyeOffsetX), static_cast<int>(bottomY - 25 + bob), 1, BLACK);
        DrawCircle(static_cast<int>(centerX + 3 + eyeOffsetX), static_cast<int>(bottomY - 25 + bob), 1, BLACK);
    }
}

/**
 * @brief Renderiza a un enemigo en el mapa con PixelArt y signos contextuales.
 *
 * - Si está en estado CHASE (persiguiendo al jugador), muestra un signo '!' rojo parpadeante sobre su cabeza.
 * - Muestra el nombre de la criatura arriba del sprite.
 */
void MapView::DrawEnemy(const Enemy& enemy) const {
    const float centerX = enemy.body.x + enemy.body.width / 2.0f;
    const float centerY = enemy.body.y + enemy.body.height / 2.0f;

    const int scale = enemy.isBoss ? 3 : 2;
    const float spriteSize = 12.0f * scale;

    DrawEllipse(
        static_cast<int>(centerX),
        static_cast<int>(enemy.body.y + enemy.body.height),
        spriteSize / 2.0f,
        4.0f,
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

    // Alerta visual de persecución
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

/**
 * @brief Renderiza a un NPC o Cartel en el supramundo.
 *
 * Si `isPromptTarget` es true (el jugador está en rango de interacción),
 * muestra una insignia flotante dorada con `[E] Leer` o `[E] Hablar`.
 */
void MapView::DrawNPC(const NPC& npc, bool isPromptTarget) const {
    const float centerX = npc.body.x + npc.body.width / 2.0f;
    const float centerY = npc.body.y + npc.body.height / 2.0f;

    const int scale = 2;
    const float spriteSize = 12.0f * scale;

    DrawEllipse(
        static_cast<int>(centerX),
        static_cast<int>(npc.body.y + npc.body.height),
        spriteSize / 2.0f,
        4.0f,
        Fade(BLACK, 0.30f)
    );

    DrawPixelArt(
        npc.art,
        centerX - spriteSize / 2.0f,
        centerY - spriteSize / 2.0f - 4.0f,
        scale,
        npc.bodyColor,
        npc.lightColor
    );

    if (isPromptTarget) {
        const char* prompt = npc.isSign ? "[E] Leer" : "[E] Hablar";
        const int pw = MeasureText(prompt, 11);
        const Rectangle badge{
            centerX - (pw + 14) / 2.0f,
            centerY - spriteSize / 2.0f - 24.0f,
            static_cast<float>(pw + 14),
            18.0f
        };
        DrawRectangleRec(badge, Color{ 18, 24, 38, 230 });
        DrawRectangleLinesEx(badge, 1.5f, GOLD);
        DrawText(prompt, static_cast<int>(badge.x + 7), static_cast<int>(badge.y + 3), 11, GOLD);
    }
}

/**
 * @brief Renderiza el mundo 2D completo bajo la cámara activa con optimización de frustum culling.
 */
void MapView::DrawWorld(
    const TileMap& map,
    const Player& player,
    const std::vector<Enemy>& enemies,
    const std::vector<NPC>& npcs,
    const Camera2D& camera,
    const NPC* nearbyNpc
) const {
    ClearBackground(BLACK);

    BeginMode2D(camera);

    // Culling de casillas según el encuadre visible de la cámara
    const float halfW = 480.0f;
    const float halfH = 270.0f;
    const int colStart = std::max(0, static_cast<int>((camera.target.x - halfW) / TileMap::TILE) - 1);
    const int colEnd = std::min(TileMap::COLS - 1, static_cast<int>((camera.target.x + halfW) / TileMap::TILE) + 1);
    const int rowStart = std::max(0, static_cast<int>((camera.target.y - halfH) / TileMap::TILE) - 1);
    const int rowEnd = std::min(TileMap::ROWS - 1, static_cast<int>((camera.target.y + halfH) / TileMap::TILE) + 1);

    for (int row = rowStart; row <= rowEnd; ++row) {
        for (int col = colStart; col <= colEnd; ++col) {
            DrawTile(map, col, row);
        }
    }

    // Dibujado de NPCs y carteles en el mundo
    for (const auto& npc : npcs) {
        DrawNPC(npc, nearbyNpc == &npc);
    }

    // Dibujado de enemigos no derrotados
    for (const Enemy& enemy : enemies) {
        if (!enemy.defeated) {
            DrawEnemy(enemy);
        }
    }

    // Dibujado del héroe
    DrawPlayer(player);

    EndMode2D();
}

/**
 * @brief Dibuja la barra de información inferior en coordenadas fijas de pantalla (no afectada por cámara 2D).
 */
void MapView::DrawHUD(const Player& player) const {
    DrawRectangle(0, 512, 960, 28, { 18, 22, 32, 245 });
    DrawRectangleLinesEx({ 0.0f, 512.0f, 960.0f, 28.0f }, 1.0f, { 60, 75, 105, 200 });

    DrawText(
        TextFormat(
            "[%s] Nv.%d (EXP %d/%d)  HP %d/%d  MP %d/%d  Pociones: %d",
            player.GetClassName(),
            player.level,
            player.exp,
            player.expNext,
            player.hp,
            player.maxHp,
            player.mp,
            player.maxMp,
            player.potions
        ),
        16,
        518,
        15,
        GOLD
    );

    DrawText(
        "WASD: Mover  |  E: Interactuar  |  P/ESC: Pausa  |  M: Minimapa",
        490,
        520,
        13,
        LIGHTGRAY
    );
}

/**
 * @brief Renderiza el minimapa en tiempo real con visor de encuadre de cámara.
 *
 * Dimensiones: 60 columnas x 3 px = 180 px; 32 filas x 3 px = 96 px.
 * Posición: Esquina superior derecha (X=768, Y=12).
 */
void MapView::DrawMinimap(
    const TileMap& map,
    const Player& player,
    const std::vector<Enemy>& enemies,
    const Camera2D& camera
) const {
    constexpr int scale = 3;

    const int mapWidth = TileMap::COLS * scale;   // 60 * 3 = 180 px
    const int mapHeight = TileMap::ROWS * scale;  // 32 * 3 = 96 px
    const int originX = 960 - mapWidth - 12;      // 768
    const int originY = 12;

    // Marco exterior con borde dorado
    DrawRectangle(
        originX - 4,
        originY - 4,
        mapWidth + 8,
        mapHeight + 8,
        Fade(Color{ 14, 20, 32, 255 }, 0.85f)
    );
    DrawRectangleLinesEx(
        { static_cast<float>(originX - 4), static_cast<float>(originY - 4),
          static_cast<float>(mapWidth + 8), static_cast<float>(mapHeight + 8) },
        1.5f,
        GOLD
    );

    // Dibujado de casillas a escala en el minimapa
    for (int row = 0; row < TileMap::ROWS; ++row) {
        for (int col = 0; col < TileMap::COLS; ++col) {
            const char tile = map.At(col, row);
            const int miniSector = (col < 17) ? 0 : (col < 29) ? 1 : (col < 40) ? 2 : 3;

            Color color;
            if (miniSector == 0)      color = { 70, 150, 80, 255 };  // Aldea
            else if (miniSector == 1) color = { 90, 175, 80, 255 };  // Pradera
            else if (miniSector == 2) color = { 40, 95, 50, 255 };   // Bosque
            else                      color = { 105, 110, 120, 255 }; // Ruinas

            const bool isGateTile = (col == 28 || col == 39) && (row == 14 || row == 15);

            if (tile == '#') {
                color = (miniSector == 3) ? Color{ 60, 65, 75, 255 } : Color{ 25, 75, 40, 255 };
            }
            else if (tile == 'p') {
                if (isGateTile) {
                    color = { 60, 220, 120, 255 }; // Compuerta abierta destacada en esmeralda
                }
                else {
                    color = (miniSector == 3) ? Color{ 160, 165, 175, 255 } : Color{ 225, 205, 145, 255 };
                }
            }
            else if (tile == 'W') {
                color = (miniSector >= 2) ? Color{ 55, 80, 130, 255 } : Color{ 70, 110, 195, 255 };
            }
            else if (tile == 'H') {
                color = (miniSector == 3) ? Color{ 80, 85, 95, 255 } : Color{ 195, 115, 85, 255 };
            }
            else if (tile == '1' || tile == '2') {
                color = { 230, 45, 45, 255 }; // Compuerta cerrada resaltada en rojo brillante
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

    // Enemigos en minimapa (rojos, jefe con borde dorado)
    for (const Enemy& enemy : enemies) {
        if (!enemy.defeated) {
            const Vector2 center = enemy.Center();
            const int miniX = originX + static_cast<int>(center.x / (TileMap::TILE) * scale);
            const int miniY = originY + static_cast<int>(center.y / (TileMap::TILE) * scale);

            if (enemy.isBoss) {
                DrawRectangle(miniX - 2, miniY - 2, 5, 5, GOLD);
                DrawRectangle(miniX - 1, miniY - 1, 3, 3, RED);
            }
            else {
                DrawRectangle(miniX - 1, miniY - 1, 3, 3, RED);
            }
        }
    }

    // Jugador en minimapa (punto blanco)
    const Vector2 playerCenter = player.Center();
    const int playerX = originX + static_cast<int>(playerCenter.x / (TileMap::TILE) * scale);
    const int playerY = originY + static_cast<int>(playerCenter.y / (TileMap::TILE) * scale);
    DrawRectangle(playerX - 2, playerY - 2, 5, 5, WHITE);

    // Visor de la cámara en el minimapa (recuadro proporcional del encuadre visible)
    const float viewW = 960.0f;
    const float viewH = 540.0f;
    const float totalWorldW = static_cast<float>(TileMap::COLS * TileMap::TILE);
    const float totalWorldH = static_cast<float>(TileMap::ROWS * TileMap::TILE);

    const float viewLeft = camera.target.x - viewW / 2.0f;
    const float viewTop = camera.target.y - viewH / 2.0f;

    const float miniViewX = originX + (viewLeft / totalWorldW) * mapWidth;
    const float miniViewY = originY + (viewTop / totalWorldH) * mapHeight;
    const float miniViewW = (viewW / totalWorldW) * mapWidth;
    const float miniViewH = (viewH / totalWorldH) * mapHeight;

    DrawRectangleLinesEx(
        { miniViewX, miniViewY, miniViewW, miniViewH },
        1.0f,
        Fade(RAYWHITE, 0.70f)
    );
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
        const bool isSelected = (i == selected);

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
