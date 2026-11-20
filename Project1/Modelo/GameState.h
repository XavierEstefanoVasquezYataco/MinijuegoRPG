/**
 * @file GameState.h
 * @brief Definiciones de enumeraciones para estados globales, fases de combate y tipos de entidad.
 *
 * Capa de Arquitectura: Modelo (Model) - Dominio y Tipos Base.
 *
 * Propósito y Responsabilidades:
 * - Centralizar los tipos de datos enumerados (`enum class`) que rigen la máquina de estados
 *   del juego, el flujo de combate por turnos, las clases de héroe y la inteligencia artificial.
 * - Evitar el uso de números mágicos o cadenas de texto para representar estados lógicos.
 *
 * Relaciones Principales:
 * - Utilizado por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) para dirigir las transiciones de pantalla.
 * - Utilizado por el Modelo del Jugador ([Modelo/Player.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/Player.h)) para arquetipos y orientación.
 * - Utilizado por las Vistas ([Vistas/TitleView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/TitleView.h), [Vistas/CombatUI.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatUI.h)) para adaptar menús y botones según el estado activo.
 */

#pragma once

/**
 * @enum GameState
 * @brief Representa la máquina de estados de alto nivel de toda la aplicación.
 *
 * Controla qué escena se actualiza y se dibuja en cada iteración del bucle principal:
 * - TITLE_SCREEN: Menú de inicio (Nueva partida, Continuar, Controles, Salir).
 * - CLASS_SELECT: Pantalla de selección entre Guerrero, Maga y Exploradora.
 * - CONTROLS: Pantalla informativa que lista los atajos de teclado y ratón.
 * - MAP: Modo supramundo; exploración 2D, movimiento y cámara en tiempo real.
 * - DIALOGUE: Modo de interacción con NPCs o lectura de carteles (pausa el movimiento).
 * - COMBAT: Escena de batalla por turnos contra un enemigo.
 * - PAUSED: Menú superpuesto de pausa (Reanudar, Reiniciar, Volver al título).
 * - GAME_OVER: Pantalla de derrota cuando la salud del héroe llega a 0.
 * - VICTORY: Pantalla final de victoria tras vencer al Dragón en las Ruinas.
 */
enum class GameState {
    TITLE_SCREEN,
    CLASS_SELECT,
    CONTROLS,
    MAP,
    DIALOGUE,
    COMBAT,
    PAUSED,
    GAME_OVER,
    VICTORY
};

/**
 * @enum HeroClass
 * @brief Identifica los tres arquetipos de personaje seleccionables por el jugador.
 *
 * Determina estadísticas base (HP, MP, ATK, DEF), velocidad de exploración,
 * tipo y coste de habilidad especial, sprite asignado y probabilidad de huida:
 * - WARRIOR: Alta salud y defensa, golpe fuerte físico cuerpo a cuerpo (50% huida).
 * - MAGE: Alto daño arcano y maná abundante, baja vitalidad (50% huida).
 * - SCOUT: Estadísticas balanceadas, +15% de velocidad en mapa y 65% de probabilidad de huida.
 */
enum class HeroClass {
    WARRIOR,
    MAGE,
    SCOUT
};

/**
 * @enum CombatPhase
 * @brief Sub-estados secuenciales del combate por turnos.
 *
 * - PLAYER_TURN: El jugador tiene el control del menú (Atacar, Habilidad, Poción, Huir).
 * - ENEMY_WAIT: Período de retardo (timer de 1s) para que el jugador lea el resultado
 *               antes de que el enemigo ejecute su contraataque.
 * - END_WAIT: Retardo final tras la derrota, victoria o escape antes de volver al mapa o fin de juego.
 */
enum class CombatPhase {
    PLAYER_TURN,
    ENEMY_WAIT,
    END_WAIT
};

/**
 * @enum CombatResult
 * @brief Resultado definitivo del encuentro de combate.
 *
 * Permite a `Game::FinishCombat()` saber qué transición realizar:
 * - NONE: Batalla en curso.
 * - PLAYER_WON: Enemigo derrotado (recompensa EXP, compuertas y verificación de jefe).
 * - PLAYER_LOST: El jugador murió (transición a GAME_OVER).
 * - FLED: Huida exitosa (transición a MAP con retroceso y cooldown de encuentro).
 */
enum class CombatResult {
    NONE,
    PLAYER_WON,
    PLAYER_LOST,
    FLED
};

/**
 * @enum PlayerAction
 * @brief Acciones disponibles en la grilla 2x2 del menú de combate.
 *
 * Los valores numéricos coinciden con los índices de los botones en `CombatUI`:
 * - ATTACK (0): Ataque físico estándar dependiente de ATK y DEF del objetivo.
 * - SKILL (1): Habilidad de clase con consumo de MP y multiplicador de daño elevado.
 * - POTION (2): Consume 1 poción del inventario y restaura hasta 40 puntos de salud.
 * - FLEE (3): Intento probabilístico de escapar del combate.
 * - NONE (-1): Ninguna acción seleccionada en este fotograma.
 */
enum class PlayerAction {
    ATTACK = 0,
    SKILL = 1,
    POTION = 2,
    FLEE = 3,
    NONE = -1
};

/**
 * @enum AIState
 * @brief Máquina de estados finita para la IA de enemigos en el supramundo.
 *
 * - PATROL: El monstruo deambula aleatoriamente cerca de su punto de aparición (`home`).
 * - CHASE: Si el jugador entra en su radio de detección (`detectRadius`), el monstruo
 *          se mueve directamente hacia las coordenadas del jugador.
 */
enum class AIState {
    PATROL,
    CHASE
};

/**
 * @enum Facing
 * @brief Dirección cardinal en la que mira el jugador.
 *
 * Sus valores numéricos (0..3) corresponden con las filas del sprite-sheet (`hero.png`)
 * o la orientación de los ojos en el renderizado procedimental:
 * - DOWN (0): Mirando hacia abajo (sur).
 * - LEFT (1): Mirando hacia la izquierda (oeste).
 * - RIGHT (2): Mirando hacia la derecha (este).
 * - UP (3): Mirando hacia arriba (norte).
 */
enum class Facing {
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3
};
