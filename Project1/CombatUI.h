#pragma once
#include <deque>
#include <string>
#include "raylib.h"
#include "Player.h"
#include "GameState.h"

class CombatUI {
public:
    CombatUI();

    void Draw(const Player& player, const Enemy& enemy,
        const std::deque<std::string>& log,
        int selected, bool menuEnabled, int hovered = -1) const;

    int GetHoveredButton(Vector2 point) const;
    PlayerAction GetActionAt(Vector2 point) const;
    Rectangle GetButtonRect(int index) const;

private:
    Rectangle buttons[4];   // 0 Atacar, 1 Habilidad, 2 Pocion, 3 Huir (grilla 2x2)

    void DrawHpBar(Rectangle r, int value, int maxValue) const;
    void DrawMpBar(Rectangle r, int value, int maxValue) const;
    void DrawExpBar(Rectangle r, int value, int maxValue) const;
    void DrawStatusBox(Rectangle r, const Combatant& c, bool showMp) const;
    void DrawPlayerStatusBox(Rectangle r, const Player& p) const;
};
