#include "CombatUI.h"
#include "PixelArt.h"
#include <algorithm>
#include <cmath>

static const char* LABELS[4] = { "ATACAR", "HABILIDAD", "POCION", "HUIR" };
static const Color CREAM{ 248, 240, 208, 255 };
static const Color INK{ 50, 55, 60, 255 };

CombatUI::CombatUI() {
    for (int i = 0; i < 4; ++i) {
        int col = i % 2, row = i / 2;
        buttons[i] = { 610.0f + col * 170.0f, 420.0f + row * 52.0f, 160.0f, 46.0f };
    }
}

void CombatUI::DrawHpBar(Rectangle r, int v, int mx) const {
    float ratio = mx > 0 ? std::clamp(static_cast<float>(v) / mx, 0.0f, 1.0f) : 0.0f;
    Color c = ratio > 0.5f ? Color{ 60, 200, 80, 255 }
        : ratio > 0.2f ? Color{ 240, 200, 40, 255 }
    : Color{ 220, 60, 50, 255 };
    DrawRectangleRounded(r, 0.5f, 6, { 70, 70, 80, 255 });
    if (ratio > 0)
        DrawRectangleRounded({ r.x, r.y, std::max(r.width * ratio, r.height), r.height }, 0.5f, 6, c);
}

void CombatUI::DrawMpBar(Rectangle r, int v, int mx) const {
    float ratio = mx > 0 ? std::clamp(static_cast<float>(v) / mx, 0.0f, 1.0f) : 0.0f;
    DrawRectangleRounded(r, 0.5f, 6, { 70, 70, 80, 255 });
    if (ratio > 0)
        DrawRectangleRounded({ r.x, r.y, std::max(r.width * ratio, r.height), r.height }, 0.5f, 6,
            { 60, 120, 230, 255 });
}

void CombatUI::DrawStatusBox(Rectangle r, const Combatant& c, bool showMp) const {
    DrawRectangleRec(r, CREAM);
    DrawRectangleLinesEx(r, 4, INK);
    DrawText(TextToUpper(c.name.c_str()), static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 10, 22, INK);
    const char* lv = TextFormat("Nv.%d", c.level);
    DrawText(lv, static_cast<int>(r.x + r.width) - 14 - MeasureText(lv, 22),
        static_cast<int>(r.y) + 10, 22, INK);

    DrawText("HP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 42, 16, { 210, 140, 20, 255 });
    DrawHpBar({ r.x + 44, r.y + 42, r.width - 60, 14 }, c.hp, c.maxHp);
    const char* nums = TextFormat("%d / %d", c.hp, c.maxHp);
    DrawText(nums, static_cast<int>(r.x + r.width) - 14 - MeasureText(nums, 16),
        static_cast<int>(r.y) + 60, 16, INK);

    if (showMp) {
        DrawText("MP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 78, 16, { 40, 90, 200, 255 });
        DrawMpBar({ r.x + 44, r.y + 78, r.width - 60, 14 }, c.mp, c.maxMp);
    }
}

void CombatUI::Draw(const Player& player, const Enemy& enemy,
    const std::deque<std::string>& log,
    int selected, bool menuEnabled) const {
    // ----- Escenario -----
    DrawRectangleGradientV(0, 0, 960, 240, { 120, 200, 245, 255 }, { 210, 238, 250, 255 });
    DrawRectangleGradientV(0, 230, 960, 180, { 120, 200, 105, 255 }, { 70, 150, 80, 255 });

    // Plataformas
    DrawEllipse(740, 255, 140, 30, { 160, 215, 135, 255 });
    DrawEllipse(240, 395, 160, 34, { 150, 210, 125, 255 });

    // Sprites con pequeno vaiven
    float bob = std::sin(static_cast<float>(GetTime()) * 3.0f) * 3.0f;
    int es = enemy.isBoss ? 13 : 10;
    float esz = 12.0f * es;
    DrawPixelArt(enemy.art, 740 - esz / 2, 255 - esz + 10 + bob, es, enemy.bodyColor, enemy.lightColor);
    DrawPixelArt(ART_HERO_BACK, 240 - 72, 395 - 144 + 12, 12, { 90, 170, 240, 255 }, WHITE);

    // ----- Cajas de estado (esquinas) -----
    DrawStatusBox({ 20, 20, 340, 80 }, enemy, false);
    DrawStatusBox({ 600, 270, 340, 104 }, player, true);

    DrawText(TextFormat("Habilidad: %d MP  |  Pociones: %d", Player::SKILL_COST, player.potions),
        610, 388, 16, WHITE);

    // ----- Panel inferior -----
    DrawRectangle(0, 400, 960, 140, { 30, 35, 50, 255 });

    // Log de eventos (izquierda)
    Rectangle logBox{ 10, 410, 580, 120 };
    DrawRectangleRec(logBox, { 40, 70, 100, 255 });
    DrawRectangleLinesEx(logBox, 4, { 215, 175, 70, 255 });
    int start = static_cast<int>(log.size()) > 4 ? static_cast<int>(log.size()) - 4 : 0;
    int y = 422;
    for (int i = start; i < static_cast<int>(log.size()); ++i) {
        DrawText(log[i].c_str(), 28, y, 20, i == static_cast<int>(log.size()) - 1 ? WHITE : LIGHTGRAY);
        y += 26;
    }

    // Menu 2x2 (derecha)
    Rectangle menuBox{ 600, 410, 350, 120 };
    DrawRectangleRec(menuBox, { 250, 250, 250, 255 });
    DrawRectangleLinesEx(menuBox, 4, { 110, 100, 160, 255 });

    Vector2 mouse = GetMousePosition();
    int hover = GetHoveredButton(mouse);
    for (int i = 0; i < 4; ++i) {
        bool sel = (i == selected) || (i == hover);
        if (sel && menuEnabled)
            DrawRectangleRounded({ buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height },
                0.3f, 6, { 255, 225, 120, 255 });
        Color txt = menuEnabled ? INK : GRAY;
        if (sel && menuEnabled) {
            float ax = buttons[i].x + 10, ay = buttons[i].y + 15;
            DrawTriangle({ ax, ay }, { ax, ay + 16 }, { ax + 12, ay + 8 }, INK);
        }
        DrawText(LABELS[i], static_cast<int>(buttons[i].x) + 30, static_cast<int>(buttons[i].y) + 12, 20, txt);
    }
}

int CombatUI::GetHoveredButton(Vector2 mouse) const {
    for (int i = 0; i < 4; ++i)
        if (CheckCollisionPointRec(mouse, buttons[i])) return i;
    return -1;
}

PlayerAction CombatUI::GetClickedAction(Vector2 mouse) const {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return PlayerAction::NONE;
    int i = GetHoveredButton(mouse);
    return i >= 0 ? static_cast<PlayerAction>(i) : PlayerAction::NONE;
}