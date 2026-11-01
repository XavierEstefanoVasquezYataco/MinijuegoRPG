#pragma once

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

enum class HeroClass {
    WARRIOR,
    MAGE,
    SCOUT
};

enum class CombatPhase { PLAYER_TURN, ENEMY_WAIT, END_WAIT };

enum class CombatResult { NONE, PLAYER_WON, PLAYER_LOST, FLED };

enum class PlayerAction { ATTACK = 0, SKILL = 1, POTION = 2, FLEE = 3, NONE = -1 };

// IA del enemigo en el mapa
enum class AIState { PATROL, CHASE };

// 0 abajo, 1 izquierda, 2 derecha, 3 arriba (filas del sprite-sheet)
enum class Facing { DOWN = 0, LEFT = 1, RIGHT = 2, UP = 3 };
