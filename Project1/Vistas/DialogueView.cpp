/**
 * @file DialogueView.cpp
 * @brief Implementación del renderizado de la ventana de diálogos y salto de línea dinámico.
 *
 * Capa de Arquitectura: Vista (View) - Interfaz de Diálogos.
 *
 * Propósito y Responsabilidades:
 * - Implementar el algoritmo de ajuste automático de líneas basado en `MeasureText`.
 * - Renderizar la caja de diálogo de 860x155 px con marco dorado y avatar PixelArt de 60x60 px.
 * - Renderizar los botones interactivos de decisión para la Curandera.
 */

#include "DialogueView.h"
#include "PixelArt.h"
#include <cmath>
#include <sstream>

/**
 * @brief Retorna las coordenadas de pantalla para los botones de confirmación Si / No.
 *
 * - Opción 0 ([S] SI): Ubicado en X=480, Y=442, ancho=130, alto=36.
 * - Opción 1 ([N] NO): Ubicado en X=640, Y=442, ancho=130, alto=36.
 */
Rectangle DialogueView::GetChoiceButtonRect(int index) const {
    if (index == 0) {
        return { 480.0f, 442.0f, 130.0f, 36.0f }; // [ SI ]
    }
    return { 640.0f, 442.0f, 130.0f, 36.0f };     // [ NO ]
}

/**
 * @brief Comprueba colisión del cursor del ratón contra los botones de opción.
 */
int DialogueView::GetHoveredChoice(Vector2 mouse) const {
    if (CheckCollisionPointRec(mouse, GetChoiceButtonRect(0))) return 0;
    if (CheckCollisionPointRec(mouse, GetChoiceButtonRect(1))) return 1;
    return -1;
}

/**
 * @brief Algoritmo didáctico de ajuste de texto automático por palabras (Word Wrapping).
 *
 * Proceso:
 * 1. Descompone la oración en palabras individuales mediante `std::stringstream`.
 * 2. Concatena palabra a palabra en una línea de prueba `testLine`.
 * 3. Si `MeasureText(testLine)` excede el límite `maxW`, dibuja la línea acumulada
 *    y comienza una nueva línea en `currentY + fontSize + 6`.
 * 4. Al finalizar el flujo de palabras, dibuja el remanente en la última posición Y calculada.
 */
void DialogueView::DrawWrappedText(const char* text, int posX, int posY, int maxW, int fontSize, Color color) const {
    std::string str(text);
    std::string line = "";
    std::stringstream ss(str);
    std::string word;
    int currentY = posY;

    while (ss >> word) {
        std::string testLine = line.empty() ? word : (line + " " + word);
        if (MeasureText(testLine.c_str(), fontSize) > maxW) {
            DrawText(line.c_str(), posX, currentY, fontSize, color);
            line = word;
            currentY += fontSize + 6;
        }
        else {
            line = testLine;
        }
    }
    if (!line.empty()) {
        DrawText(line.c_str(), posX, currentY, fontSize, color);
    }
}

/**
 * @brief Dibuja la ventana de diálogo activa, el nombre del hablante, su avatar y botones.
 *
 * Capas de renderizado:
 * 1. Rectángulo contenedor principal en Y=345 px con relleno semitransparente oscuro y doble borde dorado.
 * 2. Placa con el nombre del personaje que sobresale en la esquina superior izquierda.
 * 3. Marco y retrato PixelArt escalado x5 (60x60 px).
 * 4. Bloque de texto ajustado con interlineado legible.
 * 5. Si `isConfirmation` es true: botones `[S] SI` y `[N] NO` con resaltado dorado.
 * 6. Si no es confirmación: aviso de continuación y flecha indicadora parpadeante animada con `sin(GetTime())`.
 */
void DialogueView::Draw(const NPC& npc, const std::vector<std::string>& lines, int currentLine,
    bool isConfirmation, int selectedChoice, int hoveredChoice) const {
    if (currentLine < 0 || currentLine >= static_cast<int>(lines.size())) return;

    // 1. Caja de diálogo en la zona inferior de la pantalla (960x540)
    const Rectangle box{ 50.0f, 345.0f, 860.0f, 155.0f };

    // Fondo oscuro translúcido
    DrawRectangleRec(box, Color{ 16, 22, 34, 245 });
    // Marco dorado elegante
    DrawRectangleLinesEx(box, 3.0f, GOLD);
    DrawRectangleLinesEx({ box.x + 3.0f, box.y + 3.0f, box.width - 6.0f, box.height - 6.0f }, 1.0f, Color{ 120, 100, 50, 180 });

    // 2. Etiqueta del nombre del hablante o cartel
    const char* speaker = npc.name.c_str();
    const int nameW = MeasureText(speaker, 18);
    const Rectangle nameBadge{ box.x + 20.0f, box.y - 15.0f, static_cast<float>(nameW + 30), 28.0f };
    DrawRectangleRec(nameBadge, Color{ 26, 36, 56, 255 });
    DrawRectangleLinesEx(nameBadge, 2.0f, GOLD);
    DrawText(speaker, static_cast<int>(nameBadge.x + 15), static_cast<int>(nameBadge.y + 5), 18, GOLD);

    // 3. Marco del avatar a la izquierda
    const Rectangle avatarBox{ box.x + 20.0f, box.y + 24.0f, 80.0f, 95.0f };
    DrawRectangleRec(avatarBox, Color{ 24, 32, 48, 255 });
    DrawRectangleLinesEx(avatarBox, 2.0f, Color{ 85, 110, 150, 200 });

    // Avatar en pixel art
    const float spriteScale = 5.0f;
    const float spriteSize = 12.0f * spriteScale;
    const float spriteX = avatarBox.x + (avatarBox.width - spriteSize) / 2.0f;
    const float spriteY = avatarBox.y + (avatarBox.height - spriteSize) / 2.0f;
    DrawPixelArt(npc.art, spriteX, spriteY, static_cast<int>(spriteScale), npc.bodyColor, npc.lightColor);

    // 4. Texto con salto de línea ajustado
    const char* text = lines[currentLine].c_str();
    const int textX = static_cast<int>(box.x + 120.0f);
    const int textY = static_cast<int>(box.y + 28.0f);
    const int textMaxW = static_cast<int>(box.width - 145.0f);
    DrawWrappedText(text, textX, textY, textMaxW, 18, RAYWHITE);

    if (isConfirmation) {
        // 5. Botones de selección Sí / No
        for (int i = 0; i < 2; ++i) {
            const Rectangle btn = GetChoiceButtonRect(i);
            const bool isHighlighted = (i == selectedChoice) || (i == hoveredChoice);

            DrawRectangleRec(btn, isHighlighted ? GOLD : Color{ 28, 38, 58, 240 });
            DrawRectangleLinesEx(btn, 2.0f, isHighlighted ? RAYWHITE : Color{ 120, 140, 175, 200 });

            const char* label = (i == 0) ? "[S]  SI" : "[N]  NO";
            const int lw = MeasureText(label, 16);
            DrawText(label,
                static_cast<int>(btn.x + (btn.width - lw) / 2),
                static_cast<int>(btn.y + 9),
                16,
                isHighlighted ? BLACK : RAYWHITE);
        }

        // Ayuda de controles para confirmación
        const char* hint = "[A/D o Flechas] Seleccionar   |   [S/N o ENTER] Confirmar   |   [ESC] Cancelar";
        DrawText(hint, static_cast<int>(box.x + 120.0f), static_cast<int>(box.y + box.height - 24), 13, { 180, 195, 215, 255 });
    }
    else {
        // 6. Indicador de avance estándar
        const char* hint = "[E o ENTER] Continuar   |   [ESC] Salir";
        const int hintW = MeasureText(hint, 13);
        DrawText(hint, static_cast<int>(box.x + box.width - hintW - 24), static_cast<int>(box.y + box.height - 24), 13, { 180, 195, 215, 255 });

        // Flechita parpadeante dorada
        if (std::sin(GetTime() * 8.0f) > 0.0f) {
            DrawText(">", static_cast<int>(box.x + box.width - 18), static_cast<int>(box.y + box.height - 25), 15, GOLD);
        }
    }
}
