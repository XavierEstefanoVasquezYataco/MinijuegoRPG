/**
 * @file MapView.h
 * @brief Vista pasiva para el renderizado del supramundo, cámara 2D, HUD, minimapa y menú de pausa.
 *
 * Capa de Arquitectura: Vista (View) - Renderizado de Exploración e Interfaz de Supramundo.
 *
 * Propósito y Responsabilidades:
 * - Renderizar las casillas visibles del mapa con frustum culling según la cámara 2D.
 * - Renderizar al héroe (mediante sprite-sheet si existe o figuras geométricas de respaldo).
 * - Renderizar enemigos en el mapa con sombreado y signo de exclamación al estar en persecución.
 * - Renderizar NPCs y carteles, mostrando un indicador flotante `[E]` si el jugador está cerca.
 * - Dibujar la barra de estado inferior (HUD) con nivel, EXP, HP, MP y pociones.
 * - Dibujar el minimapa contextual proporcional de 180x96 px con el visor de encuadre de cámara.
 * - Renderizar el menú de pausa superpuesto con botones interactivos y confirmación de salida.
 *
 * Relaciones Principales:
 * - Invocada por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) en los estados `MAP` y `PAUSED`.
 * - Consume datos de solo lectura de [Modelo/TileMap.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/TileMap.h), [Modelo/Player.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/Player.h) y [Modelo/NPC.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/NPC.h).
 * - Emplea [Vistas/PixelArt.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/PixelArt.h) para los sprites de enemigos y NPCs.
 */

#pragma once
#include <vector>
#include "raylib.h"
#include "../Modelo/TileMap.h"
#include "../Modelo/Player.h"
#include "../Modelo/NPC.h"

/**
 * @class MapView
 * @brief Gestiona la presentación visual de la exploración del mundo abierto, HUD y minimapa.
 */
class MapView {
public:
    /**
     * @brief Carga texturas opcionales de disco (ej. `assets/hero.png`).
     *
     * Debe llamarse únicamente después de `InitWindow()` de Raylib.
     */
    void Load();

    /**
     * @brief Libera los recursos de texturas cargados en memoria de vídeo.
     *
     * Debe llamarse antes de `CloseWindow()` de Raylib.
     */
    void Unload();

    /**
     * @brief Renderiza el mundo 2D completo: casillas, entidades, héroe y carteles.
     *
     * Aplica optimización de descarte de tiles fuera de pantalla (frustum culling)
     * calculando los límites visibles a partir de `camera.target`.
     *
     * @param map Matriz del terreno.
     * @param player Datos de posición, orientación y animación del héroe.
     * @param enemies Lista de enemigos para dibujar aquellos que no han sido derrotados.
     * @param npcs Lista de personajes no jugadores y carteles del supramundo.
     * @param camera Cámara 2D con desplazamiento y centrado suave sobre el jugador.
     * @param nearbyNpc Puntero al NPC más cercano en rango de interacción (muestra aviso flotante `[E]`).
     */
    void DrawWorld(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies, const std::vector<NPC>& npcs,
        const Camera2D& camera, const NPC* nearbyNpc = nullptr) const;

    /**
     * @brief Dibuja la barra de interfaz inferior fija en pantalla (HUD).
     *
     * Informa: Clase, Nivel, EXP actual/necesaria, HP actual/máximo, MP actual/máximo, pociones y atajos.
     *
     * @param player Referencia constante a los datos del jugador.
     */
    void DrawHUD(const Player& player) const;

    /**
     * @brief Renderiza el minimapa en tiempo real en la esquina superior derecha (180x96 px).
     *
     * Muestra casillas simplificadas con distinción de biomas, compuertas abiertas/cerradas,
     * enemigos en rojo, jefe final en carmesí/oro, jugador en blanco y el recuadro del visor de la cámara.
     *
     * @param map Cuadrícula del mapa.
     * @param player Héroe.
     * @param enemies Colección de enemigos activos.
     * @param camera Cámara para encuadrar la región visible.
     */
    void DrawMinimap(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies, const Camera2D& camera) const;

    /**
     * @brief Renderiza la capa translúcida de pausa y sus botones.
     * @param selected Índice de la opción seleccionada por teclado (0 Reanudar, 1 Reiniciar, 2 Salir).
     */
    void DrawPause(int selected) const;

    /**
     * @brief Comprueba qué botón del menú de pausa está bajo el cursor del ratón.
     * @param mouse Coordenadas de pantalla del cursor.
     * @return int Índice de botón (0..2) o -1 si ninguno.
     */
    int GetPauseButton(Vector2 mouse) const;

    /**
     * @brief Retorna el rectángulo de pantalla del botón de pausa indicado.
     */
    Rectangle PauseButton(int i) const;

private:
    Texture2D heroSheet{};      ///< Textura de la hoja de sprites del héroe (si existe `assets/hero.png`).
    bool hasSheet = false;      ///< Bandera que indica si se cargó exitosamente la textura externa.

    void DrawTile(const TileMap& map, int c, int r) const;
    void DrawPlayer(const Player& p) const;
    void DrawEnemy(const Enemy& e) const;
    void DrawNPC(const NPC& npc, bool isPromptTarget) const;
};
