/**
 * @file TitleView.cpp
 * @brief Implementación del renderizado de las pantallas de Título, Selección de Clase y Controles.
 *
 * Capa de Arquitectura: Vista (View) - Menús y Selección de Héroe.
 *
 * Propósito y Responsabilidades:
 * - Dibujar la pantalla de título con fondos degradados y botones interactivos.
 * - Mostrar de manera didáctica la opción "Continuar" deshabilitada indicando que requiere guardado.
 * - Renderizar las tarjetas con avatar procedimental, atributos y habilidades de los tres héroes.
 * - Ofrecer detección geométrica para interacción mediante teclado y ratón.
 */

#include "TitleView.h"
#include "../Modelo/Player.h"
#include "PixelArt.h"

/// Etiquetas de los 4 botones del menú principal.
static const char* TITLE_LABELS[4] = {
    "Nueva partida",
    "Continuar",
    "Controles",
    "Salir"
};

/**
 * @brief Constructor que calcula las dimensiones y posiciones estáticas de botones y tarjetas.
 *
 * - Botones de título: Centrados en X=330 px con ancho de 300 px y espaciado vertical de 62 px.
 * - Tarjetas de clase: 3 columnas distribuidas uniformemente entre X=65 px y X=895 px.
 */
TitleView::TitleView() {
    for (int i = 0; i < 4; ++i) {
        titleButtons[i] = { 330.0f, 215.0f + i * 62.0f, 300.0f, 48.0f };
    }
    for (int i = 0; i < 3; ++i) {
        classCards[i] = { 65.0f + i * 285.0f, 85.0f, 260.0f, 375.0f };
    }
}

/**
 * @brief Función auxiliar interna para dibujar paneles con fondo sólido y borde perimetral.
 */
void TitleView::DrawDecoratedFrame(Rectangle r, Color border, Color fill) const {
    DrawRectangleRec(r, fill);
    DrawRectangleLinesEx(r, 2.0f, border);
}

/**
 * @brief Dibuja la pantalla de título completa en resolución nativa 960x540.
 *
 * Elementos dibujados:
 * 1. Fondo degradado vertical azul noche ({ 14, 20, 32 } a { 26, 36, 56 }).
 * 2. Marco perimetral dorado elegante.
 * 3. Título principal "Mini Aventura RPG" con tipografía de 48 px y sombra proyectada.
 * 4. Subtítulo "Versión 2" en 20 px.
 * 5. Los 4 botones de acción con resaltado dorado al estar seleccionados o sobrevolados.
 * 6. Opción "Continuar": visible pero con indicación pedagógica "(Disponible al implementar guardado)".
 * 7. Leyenda de ayuda en el pie de pantalla.
 */
void TitleView::DrawTitle(int selected, bool continueAvailable, int hovered) const {
    // 1. Fondo temático azul noche
    DrawRectangleGradientV(0, 0, 960, 540, { 14, 20, 32, 255 }, { 26, 36, 56, 255 });

    // 2. Bordes exteriores dorados sutiles
    DrawRectangleLinesEx({ 18.0f, 18.0f, 924.0f, 504.0f }, 2.0f, { 190, 160, 90, 160 });
    DrawRectangleLinesEx({ 22.0f, 22.0f, 916.0f, 496.0f }, 1.0f, { 130, 110, 60, 90 });

    // 3. Título Mini Aventura RPG con sombra
    const char* mainTitle = "Mini Aventura RPG";
    const int titleW = MeasureText(mainTitle, 48);
    DrawText(mainTitle, (960 - titleW) / 2 + 2, 78, 48, { 10, 10, 15, 200 }); // sombra
    DrawText(mainTitle, (960 - titleW) / 2, 76, 48, GOLD);

    // 4. Subtítulo discreto Versión 2
    const char* sub = "Version 2";
    const int subW = MeasureText(sub, 20);
    DrawText(sub, (960 - subW) / 2, 138, 20, { 185, 200, 225, 255 });

    // 5. Botones del menú principal
    for (int i = 0; i < 4; ++i) {
        const Rectangle btn = titleButtons[i];
        const bool isSelected = (i == selected);
        const bool isHovered = (i == hovered);
        const bool isHighlighted = isSelected || isHovered;
        const bool isDisabled = (i == 1 && !continueAvailable);

        Color bgColor = isDisabled ? Color{ 26, 32, 44, 200 }
            : isHighlighted ? Color{ 215, 175, 55, 255 }
            : Color{ 34, 46, 68, 240 };

        Color borderColor = isDisabled ? Color{ 60, 65, 80, 255 }
            : isHighlighted ? RAYWHITE
            : Color{ 130, 155, 190, 255 };

        Color txtColor = isDisabled ? Color{ 120, 125, 140, 255 }
            : isHighlighted ? Color{ 18, 18, 22, 255 }
            : RAYWHITE;

        DrawRectangleRec(btn, bgColor);
        DrawRectangleLinesEx(btn, isHighlighted ? 3.0f : 2.0f, borderColor);

        // Indicador triangular a la izquierda del botón activo
        if (isHighlighted && !isDisabled) {
            DrawTriangle({ btn.x + 14, btn.y + 16 },
                { btn.x + 14, btn.y + btn.height - 16 },
                { btn.x + 24, btn.y + btn.height / 2 },
                Color{ 18, 18, 22, 255 });
        }

        if (i == 1 && isDisabled) {
            // "Continuar" visible pero desactivado con aclaración
            const int lblW = MeasureText(TITLE_LABELS[i], 18);
            DrawText(TITLE_LABELS[i], static_cast<int>(btn.x + (btn.width - lblW) / 2),
                static_cast<int>(btn.y + 8), 18, txtColor);

            const char* disTxt = "(Disponible al implementar guardado)";
            const int disW = MeasureText(disTxt, 11);
            DrawText(disTxt, static_cast<int>(btn.x + (btn.width - disW) / 2),
                static_cast<int>(btn.y + 28), 11, { 145, 150, 165, 255 });
        }
        else {
            const int lblW = MeasureText(TITLE_LABELS[i], 20);
            DrawText(TITLE_LABELS[i],
                static_cast<int>(btn.x + (btn.width - lblW) / 2),
                static_cast<int>(btn.y + 14),
                20, txtColor);
        }
    }

    // 6. Pie de pantalla con atajos
    const char* hint = "[W/S o Flechas] Navegar   |   [ENTER o Clic] Seleccionar";
    const int hintW = MeasureText(hint, 15);
    DrawText(hint, (960 - hintW) / 2, 485, 15, { 160, 175, 195, 255 });
}

/**
 * @brief Renderiza la pantalla de selección de héroe con las 3 tarjetas comparativas.
 *
 * Presenta con claridad didáctica:
 * - Avatar PixelArt escalado (escala 5, 60x60 px).
 * - Valores exactos de Salud (HP), Maná (MP), Ataque (ATK) y Defensa (DEF).
 * - Nombre de la técnica especial y su coste en MP.
 * - Probabilidad porcentual explícita de escape en combate.
 * - Botones de acción "VOLVER" y "COMENZAR".
 */
void TitleView::DrawClassSelect(int selectedClass, int hoveredClass, bool backHovered, bool startHovered) const {
    DrawRectangleGradientV(0, 0, 960, 540, { 14, 20, 32, 255 }, { 24, 34, 52, 255 });
    DrawRectangleLinesEx({ 18.0f, 18.0f, 924.0f, 504.0f }, 2.0f, { 190, 160, 90, 160 });

    const char* title = "SELECCIONA A TU HEROE";
    const int tw = MeasureText(title, 34);
    DrawText(title, (960 - tw) / 2, 30, 34, GOLD);

    for (int i = 0; i < 3; ++i) {
        const Rectangle card = classCards[i];
        const bool isSelected = (i == selectedClass);
        const bool isHovered = (i == hoveredClass);

        const auto& data = GetHeroClassData(static_cast<HeroClass>(i));

        Color cardBg = isSelected ? Color{ 36, 48, 76, 245 }
            : isHovered ? Color{ 28, 38, 60, 240 }
            : Color{ 20, 26, 40, 230 };

        Color cardBorder = isSelected ? GOLD
            : isHovered ? Color{ 230, 210, 140, 255 }
            : Color{ 85, 105, 140, 200 };

        DrawRectangleRec(card, cardBg);
        DrawRectangleLinesEx(card, isSelected ? 4.0f : 2.0f, cardBorder);

        // Nombre del arquetipo
        const int nw = MeasureText(data.name, 22);
        DrawText(data.name, static_cast<int>(card.x + (card.width - nw) / 2),
            static_cast<int>(card.y + 14), 22, isSelected ? GOLD : RAYWHITE);

        // Avatar PixelArt
        float spriteX = card.x + (card.width - 12.0f * 5.0f) / 2.0f;
        float spriteY = card.y + 42.0f;
        DrawPixelArt(data.artId, spriteX, spriteY, 5, data.bodyColor, data.lightColor);

        // Resumen descriptivo
        int descY = static_cast<int>(card.y + 110);
        DrawText(data.description, static_cast<int>(card.x + 14), descY, 11, { 185, 200, 220, 255 });

        // Cuadro de estadísticas: HP, MP, Ataque, Defensa
        int statsY = descY + 28;
        DrawRectangle(static_cast<int>(card.x + 14), statsY, static_cast<int>(card.width - 28), 92,
            Color{ 14, 18, 28, 200 });
        DrawRectangleLines(static_cast<int>(card.x + 14), statsY, static_cast<int>(card.width - 28), 92,
            Color{ 70, 85, 115, 160 });

        DrawText(TextFormat("Salud (HP):  %d", data.hp), static_cast<int>(card.x + 24), statsY + 8, 15, { 80, 220, 110, 255 });
        DrawText(TextFormat("Mana (MP):   %d", data.mp), static_cast<int>(card.x + 24), statsY + 28, 15, { 70, 140, 245, 255 });
        DrawText(TextFormat("Ataque:      %d", data.atk), static_cast<int>(card.x + 24), statsY + 48, 15, { 240, 170, 70, 255 });
        DrawText(TextFormat("Defensa:     %d", data.def), static_cast<int>(card.x + 24), statsY + 68, 15, { 200, 205, 220, 255 });

        // Habilidad, coste y efecto
        int skillY = statsY + 102;
        DrawText(TextFormat("Habilidad: %s (%d MP)", data.skillName, data.skillCost),
            static_cast<int>(card.x + 14), skillY, 13, GOLD);
        DrawText(data.skillEffect, static_cast<int>(card.x + 14), skillY + 18, 12, RAYWHITE);

        // Probabilidad de huida explícita
        int fleeY = skillY + 40;
        DrawText(TextFormat("Probabilidad de huida: %d%%", data.fleeChance),
            static_cast<int>(card.x + 14), fleeY, 13,
            data.fleeChance > 50 ? Color{ 110, 230, 160, 255 } : Color{ 190, 200, 215, 255 });

        // Insignia de confirmación visual si está seleccionada
        if (isSelected) {
            Rectangle selBadge{ card.x + 35, card.y + card.height - 30, card.width - 70, 22 };
            DrawRectangleRec(selBadge, GOLD);
            const char* sTxt = "SELECCIONADO";
            int sw = MeasureText(sTxt, 12);
            DrawText(sTxt, static_cast<int>(selBadge.x + (selBadge.width - sw) / 2),
                static_cast<int>(selBadge.y + 5), 12, BLACK);
        }
    }

    // Botón VOLVER
    const Rectangle backBtn = GetBackButtonRect(GameState::CLASS_SELECT);
    DrawRectangleRec(backBtn, backHovered ? Color{ 200, 60, 60, 255 } : Color{ 40, 50, 70, 230 });
    DrawRectangleLinesEx(backBtn, 2.0f, backHovered ? RAYWHITE : Color{ 130, 145, 170, 255 });
    const char* bTxt = "VOLVER";
    int bw = MeasureText(bTxt, 16);
    DrawText(bTxt, static_cast<int>(backBtn.x + (backBtn.width - bw) / 2),
        static_cast<int>(backBtn.y + 10), 16, RAYWHITE);

    // Botón COMENZAR para confirmar la selección
    const Rectangle startBtn = GetStartButtonRect();
    DrawRectangleRec(startBtn, startHovered ? Color{ 235, 195, 65, 255 } : Color{ 195, 155, 45, 255 });
    DrawRectangleLinesEx(startBtn, 2.0f, startHovered ? RAYWHITE : GOLD);
    const char* stTxt = "COMENZAR";
    int stw = MeasureText(stTxt, 18);
    DrawText(stTxt, static_cast<int>(startBtn.x + (startBtn.width - stw) / 2),
        static_cast<int>(startBtn.y + 10), 18, BLACK);

    // Ayuda de navegación
    const char* hint = "[A/D o Flechas] Cambiar heroe   |   [ENTER o Comenzar] Confirmar   |   [ESC] Volver";
    int hw = MeasureText(hint, 15);
    DrawText(hint, (960 - hw) / 2, 510, 15, { 160, 175, 195, 255 });
}

/**
 * @brief Renderiza la pantalla de Controles con las asignaciones reales de teclado y ratón.
 */
void TitleView::DrawControls(bool backHovered) const {
    DrawRectangleGradientV(0, 0, 960, 540, { 14, 20, 32, 255 }, { 24, 34, 52, 255 });
    DrawRectangleLinesEx({ 18.0f, 18.0f, 924.0f, 504.0f }, 2.0f, { 190, 160, 90, 160 });

    const char* title = "CONTROLES";
    const int tw = MeasureText(title, 34);
    DrawText(title, (960 - tw) / 2, 45, 34, GOLD);

    Rectangle panel{ 140.0f, 105.0f, 680.0f, 340.0f };
    DrawDecoratedFrame(panel, { 130, 155, 190, 200 }, { 22, 30, 46, 240 });

    struct ControlItem {
        const char* key;
        const char* desc;
    };

    static const ControlItem list[] = {
        { "W, A, S, D  /  Flechas", "Movimiento del heroe en el mapa en 4 direcciones." },
        { "Raton (Clic Izquierdo)",   "Seleccionar y accionar botones en combate, menus y pausa." },
        { "Flechas / WASD",           "Navegar entre las 4 acciones del menu de combate." },
        { "ENTER  /  ESPACIO",        "Confirmar la accion de combate o seleccion de menu." },
        { "P  /  ESCAPE",             "Pausar o reanudar la partida durante la exploracion." },
        { "M",                        "Alternar la visualizacion del minimapa en tiempo real." }
    };

    int y = static_cast<int>(panel.y + 24);
    for (const auto& item : list) {
        DrawRectangle(static_cast<int>(panel.x + 25), y, 220, 32, Color{ 36, 48, 72, 255 });
        DrawRectangleLines(static_cast<int>(panel.x + 25), y, 220, 32, Color{ 85, 110, 150, 200 });

        DrawText(item.key, static_cast<int>(panel.x + 35), y + 8, 14, GOLD);
        DrawText(item.desc, static_cast<int>(panel.x + 260), y + 8, 15, RAYWHITE);
        y += 48;
    }

    // Botón Volver
    const Rectangle backBtn = GetBackButtonRect(GameState::CONTROLS);
    DrawRectangleRec(backBtn, backHovered ? Color{ 200, 60, 60, 255 } : Color{ 40, 50, 70, 230 });
    DrawRectangleLinesEx(backBtn, 2.0f, backHovered ? RAYWHITE : Color{ 130, 145, 170, 255 });
    const char* bTxt = "VOLVER";
    int bw = MeasureText(bTxt, 16);
    DrawText(bTxt, static_cast<int>(backBtn.x + (backBtn.width - bw) / 2),
        static_cast<int>(backBtn.y + 10), 16, RAYWHITE);

    const char* hint = "Presiona [ESCAPE], [ENTER] o haz clic en Volver";
    int hw = MeasureText(hint, 15);
    DrawText(hint, (960 - hw) / 2, 490, 15, { 160, 175, 195, 255 });
}

int TitleView::GetTitleButton(Vector2 point) const {
    for (int i = 0; i < 4; ++i) {
        if (CheckCollisionPointRec(point, titleButtons[i])) {
            return i;
        }
    }
    return -1;
}

int TitleView::GetClassCard(Vector2 point) const {
    for (int i = 0; i < 3; ++i) {
        if (CheckCollisionPointRec(point, classCards[i])) {
            return i;
        }
    }
    return -1;
}

bool TitleView::IsOverBackButton(Vector2 point, GameState state) const {
    return CheckCollisionPointRec(point, GetBackButtonRect(state));
}

bool TitleView::IsOverStartButton(Vector2 point) const {
    return CheckCollisionPointRec(point, GetStartButtonRect());
}

Rectangle TitleView::GetTitleButtonRect(int index) const {
    if (index >= 0 && index < 4) return titleButtons[index];
    return { 0.0f, 0.0f, 0.0f, 0.0f };
}

Rectangle TitleView::GetClassCardRect(int index) const {
    if (index >= 0 && index < 3) return classCards[index];
    return { 0.0f, 0.0f, 0.0f, 0.0f };
}

Rectangle TitleView::GetBackButtonRect(GameState state) const {
    if (state == GameState::CLASS_SELECT) {
        return { 65.0f, 470.0f, 130.0f, 38.0f };
    }
    return { 415.0f, 460.0f, 130.0f, 38.0f };
}

Rectangle TitleView::GetStartButtonRect() const {
    return { 400.0f, 470.0f, 160.0f, 38.0f };
}
