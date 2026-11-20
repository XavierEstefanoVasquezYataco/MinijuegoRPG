/**
 * @file DialogueView.h
 * @brief Vista pasiva para el renderizado de cuadros de diálogo y preguntas interactivas.
 *
 * Capa de Arquitectura: Vista (View) - Interfaz de Usuario Narrativa.
 *
 * Propósito y Responsabilidades:
 * - Dibujar el cuadro inferior de conversación con marco decorativo dorado y fondo translúcido.
 * - Mostrar la etiqueta con el nombre del personaje o cartel y su avatar PixelArt.
 * - Implementar ajuste automático de líneas de texto (word wrapping) para párrafos extensos.
 * - Renderizar los botones interactivos de confirmación `[S] SÍ` / `[N] NO` para la Curandera.
 * - Proveer consultas de geometría pura (`GetChoiceButtonRect`, `GetHoveredChoice`) para el ratón.
 *
 * Relaciones Principales:
 * - Invocada por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) cuando el estado es `GameState::DIALOGUE`.
 * - Lee datos del modelo [Modelo/NPC.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/NPC.h) mediante referencias constantes (`const &`).
 * - Emplea [Vistas/PixelArt.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/PixelArt.h) para renderizar el retrato de la entidad que habla.
 */

#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "../Modelo/NPC.h"

/**
 * @class DialogueView
 * @brief Gestiona la presentación visual de conversaciones, avisos de carteles y confirmaciones.
 */
class DialogueView {
public:
    /**
     * @brief Dibuja el cuadro de diálogo completo en la parte inferior de la pantalla.
     *
     * @param npc Referencia al personaje no jugador o cartel activo.
     * @param lines Colección completa de frases del diálogo.
     * @param currentLine Índice de la frase actual a mostrar.
     * @param isConfirmation true si se deben mostrar los botones `[S] SI` y `[N] NO` (caso Curandera).
     * @param selectedChoice Opción activa por teclado (0 = Sí, 1 = No).
     * @param hoveredChoice Opción bajo el cursor del ratón (-1 si ninguna).
     */
    void Draw(const NPC& npc, const std::vector<std::string>& lines, int currentLine,
        bool isConfirmation = false, int selectedChoice = 0, int hoveredChoice = -1) const;

    /**
     * @brief Obtiene el rectángulo en pantalla de los botones de confirmación.
     * @param index 0 para el botón `[S] SI`, 1 para el botón `[N] NO`.
     * @return Rectangle Coordenadas y dimensiones en píxeles.
     */
    Rectangle GetChoiceButtonRect(int index) const;

    /**
     * @brief Comprueba qué botón de confirmación sobrevuela el ratón.
     * @param mouse Coordenadas del cursor en pantalla.
     * @return int 0 para Sí, 1 para No, -1 si no colisiona con ninguno.
     */
    int GetHoveredChoice(Vector2 mouse) const;

private:
    /**
     * @brief Algoritmo de ajuste de texto: divide una cadena larga en varias líneas según el ancho máximo.
     *
     * @param text Cadena de texto a procesar.
     * @param posX Coordenada X inicial de dibujo.
     * @param posY Coordenada Y inicial de dibujo.
     * @param maxW Ancho máximo en píxeles antes de forzar un salto de línea.
     * @param fontSize Tamaño tipográfico en puntos.
     * @param color Color del texto.
     */
    void DrawWrappedText(const char* text, int posX, int posY, int maxW, int fontSize, Color color) const;
};
