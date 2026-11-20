/**
 * @file TitleView.h
 * @brief Vista pasiva para las pantallas iniciales: Título principal, Selección de Héroe y Controles.
 *
 * Capa de Arquitectura: Vista (View) - Menús y Selección.
 *
 * Propósito y Responsabilidades:
 * - Dibujar la pantalla de título principal con sus 4 botones y subtítulo "Versión 2".
 * - Dibujar la pantalla de selección de héroe con tarjetas comparativas (Guerrero, Maga, Exploradora),
 *   resaltando estadísticas, habilidades, costes de MP, probabilidades de huida y avatares pixel art.
 * - Dibujar la pantalla de controles con la lista de atajos reales de teclado y ratón.
 * - Proveer consultas puras de colisión geométrica (`GetTitleButton`, `GetClassCard`, etc.)
 *   para que el Controlador procese el ratón sin que la vista ejecute lógica de juego.
 *
 * Relaciones Principales:
 * - Invocada por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) durante los estados `TITLE_SCREEN`, `CLASS_SELECT` y `CONTROLS`.
 * - Emplea [Vistas/PixelArt.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/PixelArt.h) para renderizar los avatares de las tres clases en las tarjetas.
 * - Consulta [Modelo/GameState.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/GameState.h) para saber qué botón de retorno ("Volver") evaluar.
 */

#pragma once
#include "raylib.h"
#include "../Modelo/GameState.h"

/**
 * @class TitleView
 * @brief Gestiona el renderizado y geometría de botones de los menús previos al inicio del juego.
 */
class TitleView {
public:
    /**
     * @brief Constructor. Inicializa las posiciones y dimensiones fijas de los botones y tarjetas.
     */
    TitleView();

    /**
     * @brief Renderiza el menú principal (Nueva partida, Continuar, Controles, Salir).
     *
     * @param selected Índice de la opción actualmente seleccionada con teclado (0..3).
     * @param continueAvailable Si es false, muestra "Continuar" deshabilitado con aviso explicativo.
     * @param hovered Índice del botón sobre el que se encuentra el cursor del ratón (-1 si ninguno).
     */
    void DrawTitle(int selected, bool continueAvailable, int hovered = -1) const;

    /**
     * @brief Renderiza las 3 tarjetas de arquetipos de héroe con sus estadísticas y habilidades.
     *
     * @param selectedClass Tarjeta actualmente elegida (0 Guerrero, 1 Maga, 2 Exploradora).
     * @param hoveredClass Tarjeta bajo el cursor del ratón (-1 si ninguna).
     * @param backHovered true si el cursor del ratón sobrevuela el botón "VOLVER".
     * @param startHovered true si el cursor del ratón sobrevuela el botón "COMENZAR".
     */
    void DrawClassSelect(int selectedClass, int hoveredClass = -1, bool backHovered = false, bool startHovered = false) const;

    /**
     * @brief Renderiza la lista detallada de combinaciones de teclas y controles del juego.
     * @param backHovered true si el cursor sobrevuela el botón "VOLVER".
     */
    void DrawControls(bool backHovered = false) const;

    /**
     * @brief Consulta qué botón del menú de título contiene las coordenadas del cursor.
     * @param point Coordenadas mundiales o de pantalla del ratón.
     * @return int Índice del botón (0..3) o -1 si el ratón está fuera de todos.
     */
    int GetTitleButton(Vector2 point) const;

    /**
     * @brief Consulta qué tarjeta de clase contiene las coordenadas del cursor.
     * @param point Coordenadas de pantalla del ratón.
     * @return int Índice de la tarjeta (0..2) o -1 si no colisiona.
     */
    int GetClassCard(Vector2 point) const;

    /**
     * @brief Comprueba si el ratón apunta sobre el botón "VOLVER" en la escena activa.
     * @param point Coordenadas del ratón.
     * @param state Estado actual (`CLASS_SELECT` o `CONTROLS`).
     * @return true Si el punto colisiona con el botón Volver correspondiente.
     */
    bool IsOverBackButton(Vector2 point, GameState state) const;

    /**
     * @brief Comprueba si el cursor del ratón apunta sobre el botón "COMENZAR".
     * @param point Coordenadas del ratón.
     */
    bool IsOverStartButton(Vector2 point) const;

    Rectangle GetTitleButtonRect(int index) const;
    Rectangle GetClassCardRect(int index) const;
    Rectangle GetBackButtonRect(GameState state) const;
    Rectangle GetStartButtonRect() const;

private:
    Rectangle titleButtons[4];  ///< Cajas rectangulares para los 4 botones del menú principal.
    Rectangle classCards[3];    ///< Cajas rectangulares para las 3 tarjetas de héroe.

    /**
     * @brief Dibuja un marco decorado con fondo y borde coloreado.
     */
    void DrawDecoratedFrame(Rectangle r, Color border, Color fill) const;
};
