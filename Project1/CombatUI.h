#pragma once
#include <deque>
#include <string>
#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "CombatPresentation.h"

class CombatUI {
public:
    CombatUI();

    void Draw(const Player& player, const Enemy& enemy,
        const std::deque<std::string>& log,
        int selected, bool menuEnabled, int hovered,
        const CombatPresentation& pres) const;

    int GetHoveredButton(Vector2 point) const;
    PlayerAction GetActionAt(Vector2 point) const;
    Rectangle GetButtonRect(int index) const;

private:
    Rectangle buttons[4];   // 0 Atacar, 1 Habilidad, 2 Pocion, 3 Huir (grilla 2x2)

    void DrawBackground(CombatBiome biome, float presentationTime) const;
    void DrawHpBar(Rectangle r, float value, int maxValue) const;
    void DrawMpBar(Rectangle r, float value, int maxValue) const;
    void DrawExpBar(Rectangle r, int value, int maxValue) const;
    void DrawStatusBox(Rectangle r, const Combatant& c, float visualHp, bool showMp, float visualMp = 0.0f) const;
    void DrawPlayerStatusBox(Rectangle r, const Player& p, float visualHp, float visualMp) const;
    void DrawSkillFX(int skillClass, float timer, Vector2 targetCenter) const;
    void DrawHealSparkles(Vector2 playerCenter, float timer) const;
};
