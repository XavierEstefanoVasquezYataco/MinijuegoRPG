/**
 * @file CombatUI.h
 * @brief Vista pasiva para la arena de combate por turnos, panel de estado y menú 2x2.
 *
 * Capa de Arquitectura: Vista (View) - Interfaz de Combate.
 *
 * Propósito y Responsabilidades:
 * - Dibujar el escenario de batalla adaptado al bioma (Pradera, Bosque, Ruinas).
 * - Renderizar los sprites del enemigo y del héroe visto de espaldas con las animaciones de `CombatPresentation`.
 * - Renderizar cajas de estado con barras animadas de vida (HP), maná (MP) y experiencia (EXP).
 * - Dibujar el registro de eventos de combate (combat log) con formato cromático semántico.
 * - Dibujar el menú de 4 comandos en grilla 2x2 (Atacar, Habilidad, Poción, Huir) con flechas indicadoras.
 * - Proveer detección geométrica pura (`GetHoveredButton`, `GetActionAt`) para delegar la decisión al Controlador.
 *
 * Relaciones Principales:
 * - Invocada por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) en el estado `GameState::COMBAT`.
 * - Recibe referencias constantes a [Modelo/Player.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/Player.h), [Modelo/GameState.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/GameState.h) y [Vistas/CombatPresentation.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatPresentation.h).
 * - Emplea [Vistas/PixelArt.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/PixelArt.h) para los sprites del héroe y del monstruo.
 */

#pragma once
#include <deque>
#include <string>
#include "raylib.h"
#include "../Modelo/Player.h"
#include "../Modelo/GameState.h"
#include "CombatPresentation.h"

/**
 * @class CombatUI
 * @brief Gestiona el renderizado de la interfaz y la arena de combate por turnos.
 */
class CombatUI {
public:
    CombatUI();

    /**
     * @brief Renderiza toda la escena de combate (fondo, combatientes, cajas de estado, log y comandos).
     *
     * @param player Referencia constante a los datos del héroe.
     * @param enemy Referencia constante a los datos del oponente.
     * @param log Cola de mensajes recientes del combate.
     * @param selected Índice de acción seleccionada por teclado (0..3).
     * @param menuEnabled true si es el turno del jugador y los botones responden; false durante turnos enemigos.
     * @param hovered Índice del botón bajo el cursor del ratón (-1 si ninguno).
     * @param pres Referencia constante al módulo de presentación para obtener barras interpoladas y desfases.
     */
    void Draw(const Player& player, const Enemy& enemy,
        const std::deque<std::string>& log,
        int selected, bool menuEnabled, int hovered,
        const CombatPresentation& pres) const;

    /**
     * @brief Detecta qué botón del menú 2x2 está bajo el cursor del ratón.
     * @param point Coordenadas de pantalla del ratón.
     * @return int Índice de botón (0..3) o -1 si no colisiona.
     */
    int GetHoveredButton(Vector2 point) const;

    /**
     * @brief Traduce las coordenadas del ratón a una acción de combate (`PlayerAction`).
     * @param point Coordenadas del ratón.
     * @return PlayerAction Acción correspondiente (ATTACK, SKILL, POTION, FLEE, o NONE).
     */
    PlayerAction GetActionAt(Vector2 point) const;

    /**
     * @brief Retorna el rectángulo de pantalla del botón especificado.
     */
    Rectangle GetButtonRect(int index) const;

private:
    Rectangle buttons[4];   ///< Rectángulos para los botones: 0 Atacar, 1 Habilidad, 2 Poción, 3 Huir.

    void DrawBackground(CombatBiome biome, float presentationTime) const;
    void DrawHpBar(Rectangle r, float value, int maxValue) const;
    void DrawMpBar(Rectangle r, float value, int maxValue) const;
    void DrawExpBar(Rectangle r, int value, int maxValue) const;
    void DrawStatusBox(Rectangle r, const Combatant& c, float visualHp, bool showMp, float visualMp = 0.0f) const;
    void DrawPlayerStatusBox(Rectangle r, const Player& p, float visualHp, float visualMp) const;
    void DrawSkillFX(int skillClass, float timer, Vector2 targetCenter) const;
    void DrawHealSparkles(Vector2 playerCenter, float timer) const;
};
