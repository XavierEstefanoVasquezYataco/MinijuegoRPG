#include "CombatUI.h"
#include "PixelArt.h"
#include <algorithm>
#include <cmath>
#include <cstring>

static const char* LABELS[4] = { "ATACAR", "HABILIDAD", "POCION", "HUIR" };
static const Color CREAM{ 248, 240, 208, 255 };
static const Color INK{ 40, 40, 55, 255 };

CombatUI::CombatUI() {
    buttons[0] = { 610, 420, 160, 46 };
    buttons[1] = { 780, 420, 160, 46 };
    buttons[2] = { 610, 474, 160, 46 };
    buttons[3] = { 780, 474, 160, 46 };
}

void CombatUI::DrawBackground(CombatBiome biome, float presentationTime) const {
    switch (biome) {
    case CombatBiome::FOREST: {
        // Cielo crepuscular sombrio
        DrawRectangleGradientV(0, 0, 960, 240, { 35, 60, 55, 255 }, { 70, 105, 85, 255 });
        // Siluetas lejanas de arboles altos
        for (int i = 0; i < 9; ++i) {
            float tx = i * 115.0f + 20.0f;
            DrawTriangle({ tx, 170 }, { tx - 30, 230 }, { tx + 30, 230 }, { 28, 48, 42, 255 });
            DrawTriangle({ tx, 150 }, { tx - 22, 200 }, { tx + 22, 200 }, { 28, 48, 42, 255 });
            DrawRectangle(static_cast<int>(tx) - 4, 230, 8, 15, { 20, 35, 30, 255 });
        }
        // Suelo musgoso
        DrawRectangleGradientV(0, 230, 960, 180, { 55, 90, 45, 255 }, { 30, 50, 25, 255 });
        // Esporas magicas flotantes suaves
        for (int s = 0; s < 8; ++s) {
            float sx = std::fmod(s * 125.0f + presentationTime * 12.0f, 960.0f);
            float sy = 240.0f + std::sin(presentationTime * 2.0f + s) * 20.0f + s * 15.0f;
            DrawCircle(static_cast<int>(sx), static_cast<int>(sy), 2.5f, { 140, 240, 160, 160 });
        }
        break;
    }

    case CombatBiome::RUINS: {
        // Cielo rojizo cenizo amenazante
        DrawRectangleGradientV(0, 0, 960, 240, { 120, 50, 45, 255 }, { 175, 95, 75, 255 });
        // Columnas de piedra derruidas al fondo
        for (int i = 0; i < 7; ++i) {
            float cx = i * 145.0f + 40.0f;
            float ch = 45.0f + (i % 3) * 20.0f;
            DrawRectangle(static_cast<int>(cx), static_cast<int>(230.0f - ch), 22, static_cast<int>(ch), { 75, 55, 55, 255 });
            DrawRectangle(static_cast<int>(cx) - 3, static_cast<int>(230.0f - ch), 28, 6, { 95, 70, 70, 255 });
        }
        // Suelo de piedra agrietada
        DrawRectangleGradientV(0, 230, 960, 180, { 65, 60, 70, 255 }, { 40, 35, 45, 255 });
        // Trazos de losas
        for (int l = 0; l < 10; ++l) {
            DrawLine(l * 100, 260 + (l % 2) * 40, l * 100 + 70, 260 + (l % 2) * 40, { 50, 45, 55, 255 });
        }
        // Chispas o brasas ascendentes tenues
        for (int e = 0; e < 6; ++e) {
            float ex = std::fmod(e * 160.0f + presentationTime * 8.0f, 960.0f);
            float ey = 380.0f - std::fmod(presentationTime * 35.0f + e * 40.0f, 160.0f);
            DrawCircle(static_cast<int>(ex), static_cast<int>(ey), 2.0f, { 255, 140, 80, 180 });
        }
        break;
    }

    case CombatBiome::VILLAGE:
    case CombatBiome::MEADOW:
    default: {
        // Cielo azul celeste
        DrawRectangleGradientV(0, 0, 960, 240, { 115, 195, 245, 255 }, { 210, 238, 250, 255 });
        // Colinas suaves
        DrawEllipse(260, 245, 360.0f, 55.0f, { 135, 210, 120, 255 });
        DrawEllipse(760, 250, 340.0f, 50.0f, { 145, 215, 130, 255 });
        // Pradera verde vibrante
        DrawRectangleGradientV(0, 230, 960, 180, { 115, 200, 100, 255 }, { 65, 145, 75, 255 });
        // Flores silvestres decorativas
        DrawCircle(140, 280, 3.0f, { 255, 235, 120, 255 });
        DrawCircle(320, 340, 3.0f, { 255, 255, 255, 255 });
        DrawCircle(520, 275, 3.0f, { 255, 235, 120, 255 });
        DrawCircle(850, 310, 3.0f, { 255, 255, 255, 255 });
        break;
    }
    }
}

void CombatUI::DrawHpBar(Rectangle r, float value, int maxValue) const {
    const float ratio = maxValue > 0 ? std::clamp(value / static_cast<float>(maxValue), 0.0f, 1.0f) : 0.0f;
    DrawRectangleRounded(r, 0.5f, 6, { 70, 70, 80, 255 });
    if (ratio > 0.0f) {
        Color hpColor = { 70, 215, 90, 255 }; // Verde estandar
        if (ratio < 0.25f) hpColor = { 230, 60, 60, 255 }; // Rojo peligro
        else if (ratio < 0.5f) hpColor = { 235, 175, 40, 255 }; // Naranja precaucion

        DrawRectangleRounded({ r.x, r.y, std::max(r.width * ratio, r.height), r.height }, 0.5f, 6, hpColor);
    }
}

void CombatUI::DrawMpBar(Rectangle r, float value, int maxValue) const {
    const float ratio = maxValue > 0 ? std::clamp(value / static_cast<float>(maxValue), 0.0f, 1.0f) : 0.0f;
    DrawRectangleRounded(r, 0.5f, 6, { 70, 70, 80, 255 });
    if (ratio > 0.0f)
        DrawRectangleRounded({ r.x, r.y, std::max(r.width * ratio, r.height), r.height }, 0.5f, 6,
            { 60, 120, 230, 255 });
}

void CombatUI::DrawExpBar(Rectangle r, int value, int maxValue) const {
    const float ratio = maxValue > 0 ? std::clamp(static_cast<float>(value) / static_cast<float>(maxValue), 0.0f, 1.0f) : 0.0f;
    DrawRectangleRounded(r, 0.5f, 6, { 70, 70, 80, 255 });
    if (ratio > 0.0f)
        DrawRectangleRounded({ r.x, r.y, std::max(r.width * ratio, r.height), r.height }, 0.5f, 6,
            { 165, 110, 230, 255 });
}

void CombatUI::DrawStatusBox(Rectangle r, const Combatant& c, float visualHp, bool showMp, float visualMp) const {
    DrawRectangleRec(r, CREAM);
    DrawRectangleLinesEx(r, 4, INK);

    DrawText(TextToUpper(c.name.c_str()), static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 8, 20, INK);
    const char* lv = TextFormat("Nv.%d", c.level);
    DrawText(lv, static_cast<int>(r.x + r.width) - 14 - MeasureText(lv, 20),
        static_cast<int>(r.y) + 8, 20, INK);

    DrawText("HP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 34, 14, { 210, 130, 20, 255 });
    DrawHpBar({ r.x + 44, r.y + 34, r.width - 60, 14 }, visualHp, c.maxHp);
    const char* hpNums = TextFormat("%d / %d", c.hp, c.maxHp);
    DrawText(hpNums, static_cast<int>(r.x + r.width) - 14 - MeasureText(hpNums, 13),
        static_cast<int>(r.y) + 50, 13, INK);

    if (showMp) {
        DrawText("MP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 64, 14, { 40, 90, 200, 255 });
        DrawMpBar({ r.x + 44, r.y + 64, r.width - 60, 14 }, visualMp, c.maxMp);
    }
}

void CombatUI::DrawPlayerStatusBox(Rectangle r, const Player& p, float visualHp, float visualMp) const {
    DrawRectangleRec(r, CREAM);
    DrawRectangleLinesEx(r, 4, INK);

    // Cabecera: Nombre y Nivel
    DrawText(TextToUpper(p.name.c_str()), static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 8, 20, INK);
    const char* lv = TextFormat("Nv.%d", p.level);
    DrawText(lv, static_cast<int>(r.x + r.width) - 14 - MeasureText(lv, 20),
        static_cast<int>(r.y) + 8, 20, INK);

    // Barra HP con tono de alerta si la salud es menor al 25%
    Color hpLabelCol = (p.hp <= p.maxHp / 4) ? Color{ 220, 50, 50, 255 } : Color{ 210, 130, 20, 255 };
    DrawText("HP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 34, 14, hpLabelCol);
    DrawHpBar({ r.x + 44, r.y + 34, r.width - 60, 12 }, visualHp, p.maxHp);
    const char* hpNums = TextFormat("%d / %d", p.hp, p.maxHp);
    DrawText(hpNums, static_cast<int>(r.x + r.width) - 14 - MeasureText(hpNums, 13),
        static_cast<int>(r.y) + 48, 13, INK);

    // Barra MP
    DrawText("MP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 64, 14, { 40, 90, 200, 255 });
    DrawMpBar({ r.x + 44, r.y + 64, r.width - 60, 12 }, visualMp, p.maxMp);
    const char* mpNums = TextFormat("%d / %d", p.mp, p.maxMp);
    DrawText(mpNums, static_cast<int>(r.x + r.width) - 14 - MeasureText(mpNums, 13),
        static_cast<int>(r.y) + 78, 13, INK);

    // Barra EXP
    DrawText("EXP", static_cast<int>(r.x) + 14, static_cast<int>(r.y) + 94, 13, { 140, 80, 210, 255 });
    DrawExpBar({ r.x + 44, r.y + 96, r.width - 60, 9 }, p.exp, p.expNext);
    const char* expNums = TextFormat("%d / %d", p.exp, p.expNext);
    DrawText(expNums, static_cast<int>(r.x + r.width) - 14 - MeasureText(expNums, 12),
        static_cast<int>(r.y) + 107, 12, INK);
}

void CombatUI::DrawSkillFX(int skillClass, float timer, Vector2 targetCenter) const {
    const float duration = 0.45f;
    const float progress = std::clamp(1.0f - (timer / duration), 0.0f, 1.0f);
    const float ex = targetCenter.x;
    const float ey = targetCenter.y;

    switch (skillClass) {
    case 0: { // GUERRERO: Golpe Fuerte (corte demoledor dorado y chispa de choque)
        const float len = 90.0f * progress;
        DrawLineEx({ ex - len, ey - len * 0.7f }, { ex + len, ey + len * 0.7f }, 7.0f, GOLD);
        DrawLineEx({ ex - len, ey - len * 0.7f }, { ex + len, ey + len * 0.7f }, 2.5f, WHITE);
        DrawCircleLines(static_cast<int>(ex), static_cast<int>(ey), 15.0f + 40.0f * progress, ORANGE);
        DrawCircleLines(static_cast<int>(ex), static_cast<int>(ey), 8.0f + 25.0f * progress, YELLOW);
        break;
    }
    case 1: { // MAGA: Centella Arcana (anillos de resonancia magica y orbes arcanos)
        const float radius = 15.0f + 55.0f * progress;
        DrawCircleLines(static_cast<int>(ex), static_cast<int>(ey), radius, { 100, 220, 255, 230 });
        DrawCircleLines(static_cast<int>(ex), static_cast<int>(ey), radius * 0.6f, { 220, 110, 255, 240 });
        for (int a = 0; a < 6; ++a) {
            const float ang = a * 1.047f + progress * 3.5f;
            const float px = ex + std::cos(ang) * (radius * 0.85f);
            const float py = ey + std::sin(ang) * (radius * 0.85f);
            DrawCircle(static_cast<int>(px), static_cast<int>(py), 5.0f, { 255, 240, 150, 255 });
        }
        break;
    }
    case 2: { // EXPLORADORA: Tiro Critico (haz penetrante esmeralda y destello critico)
        const float startX = ex - 180.0f + 140.0f * progress;
        const float endX = startX + 110.0f;
        DrawLineEx({ startX, ey }, { endX, ey }, 6.0f, { 120, 255, 140, 240 });
        DrawLineEx({ startX, ey }, { endX, ey }, 2.5f, WHITE);
        DrawCircle(static_cast<int>(ex), static_cast<int>(ey), 22.0f * (1.0f - progress), { 255, 255, 200, 255 });
        break;
    }
    default:
        break;
    }
}

void CombatUI::DrawHealSparkles(Vector2 playerCenter, float timer) const {
    const float duration = 0.5f;
    const float progress = std::clamp(1.0f - (timer / duration), 0.0f, 1.0f);
    const float px = playerCenter.x;
    const float py = playerCenter.y;

    // Destellos ascendentes en cruz verde esmeralda y oro
    for (int i = 0; i < 7; ++i) {
        float offsetAngle = i * 0.897f;
        float dist = 25.0f + std::sin(offsetAngle) * 15.0f;
        float sx = px + std::cos(offsetAngle) * dist;
        float sy = py - 40.0f * progress + (i * 8.0f);
        float size = 4.0f * (1.0f - progress);

        DrawRectangle(static_cast<int>(sx - size), static_cast<int>(sy - 1.0f), static_cast<int>(size * 2.0f), 2, { 120, 255, 140, 240 });
        DrawRectangle(static_cast<int>(sx - 1.0f), static_cast<int>(sy - size), 2, static_cast<int>(size * 2.0f), { 255, 245, 160, 255 });
    }
}

void CombatUI::Draw(const Player& player, const Enemy& enemy,
    const std::deque<std::string>& log,
    int selected, bool menuEnabled, int hovered,
    const CombatPresentation& pres) const {

    // 1. Escenario procedural segun el bioma
    DrawBackground(pres.GetBiome(), pres.GetPresentationTime());

    // 2. Plataformas adaptadas al bioma con sacudida de pantalla leve
    const Vector2 shake = pres.GetShakeOffset();
    Color platformEnemyCol = { 160, 215, 135, 255 };
    Color platformHeroCol = { 150, 210, 125, 255 };

    if (pres.GetBiome() == CombatBiome::FOREST) {
        platformEnemyCol = { 80, 125, 75, 255 };
        platformHeroCol = { 70, 110, 65, 255 };
    }
    else if (pres.GetBiome() == CombatBiome::RUINS) {
        platformEnemyCol = { 95, 90, 105, 255 };
        platformHeroCol = { 85, 80, 95, 255 };
    }

    DrawEllipse(740 + static_cast<int>(shake.x), 255 + static_cast<int>(shake.y),
        140.0f, 30.0f, platformEnemyCol);
    DrawEllipse(240 + static_cast<int>(shake.x * 0.5f), 395 + static_cast<int>(shake.y * 0.5f),
        160.0f, 34.0f, platformHeroCol);

    // 3. Sprites con animacion independiente del tiempo de sistema (congelable en pausa)
    const float bob = std::sin(pres.GetPresentationTime() * 3.0f) * 3.0f;
    const int es = enemy.isBoss ? 13 : 10;
    const float esz = 12.0f * es;

    // Sprite Enemigo con Hit Flash y Embestida (lunge)
    Color enemyBody = enemy.bodyColor;
    Color enemyLight = enemy.lightColor;
    if (pres.GetEnemyFlash() > 0.0f) {
        const bool blink = (static_cast<int>(pres.GetEnemyFlash() * 30.0f) % 2 == 0);
        if (blink) {
            enemyBody = { 255, 240, 240, 255 };
            enemyLight = { 255, 130, 130, 255 };
        }
        else {
            enemyBody = { 255, 75, 75, 255 };
            enemyLight = { 255, 200, 200, 255 };
        }
    }
    const float enemyPosX = 740.0f - esz / 2.0f + shake.x + pres.GetEnemyLungeOffset().x;
    const float enemyPosY = 255.0f - esz + 10.0f + bob + shake.y;
    DrawPixelArt(enemy.art, enemyPosX, enemyPosY, es, enemyBody, enemyLight);

    // Sprite Heroe visto de espaldas con indumentaria de clase, Player Flash y Embestida
    const auto& heroData = GetHeroClassData(player.heroClass);
    Color heroBody = heroData.bodyColor;
    Color heroLight = heroData.lightColor;
    if (pres.GetPlayerFlash() > 0.0f) {
        const bool blink = (static_cast<int>(pres.GetPlayerFlash() * 30.0f) % 2 == 0);
        if (blink) {
            heroBody = { 255, 90, 90, 255 };
            heroLight = { 255, 220, 220, 255 };
        }
    }
    const float playerPosX = 240.0f - 72.0f + shake.x * 0.5f + pres.GetHeroLungeOffset().x;
    const float playerPosY = 395.0f - 144.0f + 12.0f + shake.y * 0.5f;
    DrawPixelArt(ART_HERO_BACK, playerPosX, playerPosY, 12, heroBody, heroLight);

    // Efecto procedural de habilidad sobre el enemigo
    if (pres.GetSkillTimer() > 0.0f && pres.GetSkillClass() >= 0) {
        DrawSkillFX(pres.GetSkillClass(), pres.GetSkillTimer(), { 740.0f + shake.x, 230.0f + shake.y });
    }

    // Efecto de recuperacion de pocion (sparkles verdes y dorados sobre el heroe)
    if (pres.GetHealSparkleTimer() > 0.0f) {
        DrawHealSparkles({ playerPosX + 72.0f, playerPosY + 72.0f }, pres.GetHealSparkleTimer());
    }

    // Destello perimetral sutil al recibir dano el heroe
    if (pres.GetPlayerFlash() > 0.0f) {
        const unsigned char flashAlpha = static_cast<unsigned char>(
            std::clamp(pres.GetPlayerFlash() / 0.3f, 0.0f, 1.0f) * 60.0f
        );
        DrawRectangle(0, 0, 960, 540, { 220, 30, 30, flashAlpha });
    }

    // Numeros flotantes de combate con sombra y desvanecimiento alfa
    for (const auto& ft : pres.GetFloatingTexts()) {
        const float alphaRatio = std::clamp(ft.timer / ft.maxTimer, 0.0f, 1.0f);
        Color textColor = ft.color;
        textColor.a = static_cast<unsigned char>(alphaRatio * 255.0f);
        Color shadowColor = BLACK;
        shadowColor.a = static_cast<unsigned char>(alphaRatio * 180.0f);

        DrawText(ft.text.c_str(), static_cast<int>(ft.pos.x) + 2, static_cast<int>(ft.pos.y) + 2, ft.fontSize, shadowColor);
        DrawText(ft.text.c_str(), static_cast<int>(ft.pos.x), static_cast<int>(ft.pos.y), ft.fontSize, textColor);
    }

    // 4. Cajas de estado con barras animadas interpoladas
    DrawStatusBox({ 20, 20, 340, 80 }, enemy, pres.GetVisualEnemyHp(), false);
    DrawPlayerStatusBox({ 600, 255, 340, 125 }, player, pres.GetVisualPlayerHp(), pres.GetVisualPlayerMp());

    DrawText(TextFormat("%s: %d MP  |  Pociones: %d", player.GetSkillName(), player.GetSkillCost(), player.potions),
        610, 386, 15, WHITE);

    // 5. Panel inferior (960x140 en coordenadas fijas de pantalla)
    DrawRectangle(0, 400, 960, 140, { 30, 35, 50, 255 });

    // Log de eventos (izquierda) con colores semanticos
    Rectangle logBox{ 10, 410, 580, 120 };
    DrawRectangleRec(logBox, { 40, 70, 100, 255 });
    DrawRectangleLinesEx(logBox, 4, { 215, 175, 70, 255 });
    int start = static_cast<int>(log.size()) > 4 ? static_cast<int>(log.size()) - 4 : 0;
    int y = 422;
    for (int i = start; i < static_cast<int>(log.size()); ++i) {
        const std::string& line = log[i];
        Color textColor = (i == static_cast<int>(log.size()) - 1) ? WHITE : LIGHTGRAY;

        if (line.find("SUBIDA DE NIVEL") != std::string::npos) {
            textColor = { 255, 235, 100, 255 }; // Oro brillante
        }
        else if (line.find("HP max") != std::string::npos || line.find("Mejoras") != std::string::npos) {
            textColor = { 200, 160, 255, 255 }; // Amatista
        }
        else if (line.find("ha sido derrotado") != std::string::npos || line.find("pts de EXP") != std::string::npos) {
            textColor = { 130, 240, 170, 255 }; // Verde victoria
        }
        else if (line.find("Recupera") != std::string::npos) {
            textColor = { 120, 250, 140, 255 }; // Verde sanacion
        }
        else if (line.find("critico") != std::string::npos || line.find("CRITICO") != std::string::npos) {
            textColor = { 255, 215, 80, 255 }; // Oro critico
        }
        else if (line.find("golpea causando") != std::string::npos) {
            textColor = { 255, 150, 140, 255 }; // Rojo golpe recibido
        }

        DrawText(line.c_str(), 28, y, 20, textColor);
        y += 26;
    }

    // 6. Menu 2x2 (derecha)
    Rectangle menuBox{ 600, 410, 350, 120 };
    DrawRectangleRec(menuBox, { 250, 250, 250, 255 });
    DrawRectangleLinesEx(menuBox, 4, { 110, 100, 160, 255 });

    for (int i = 0; i < 4; ++i) {
        bool sel = (i == selected) || (i == hovered);
        if (sel && menuEnabled) {
            DrawRectangleRounded({ buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height },
                0.3f, 6, { 255, 225, 120, 255 });
            DrawRectangleRoundedLinesEx({ buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height },
                0.3f, 6, 2.0f, { 180, 130, 40, 255 });
        }

        Color txt = menuEnabled ? INK : GRAY;

        // Feedback contextual en las etiquetas de los botones sin desbordar
        if (i == 1 && !player.CanUseSkill() && menuEnabled) {
            txt = { 160, 80, 80, 255 }; // Alerta falta de mana
        }
        else if (i == 2 && player.potions <= 0 && menuEnabled) {
            txt = { 150, 150, 150, 255 }; // Sin existencias de pociones
        }

        if (sel && menuEnabled) {
            float ax = buttons[i].x + 8, ay = buttons[i].y + 15;
            DrawTriangle({ ax, ay }, { ax, ay + 16 }, { ax + 10, ay + 8 }, INK);
        }

        const char* label = (i == 1) ? player.GetSkillName() : LABELS[i];
        int fontSize = (i == 1 && std::strlen(label) > 12) ? 16 : 18;
        int textOffset = (sel && menuEnabled) ? 24 : 16;
        DrawText(label, static_cast<int>(buttons[i].x) + textOffset, static_cast<int>(buttons[i].y) + 14, fontSize, txt);
    }
}

int CombatUI::GetHoveredButton(Vector2 point) const {
    for (int i = 0; i < 4; ++i) {
        if (CheckCollisionPointRec(point, buttons[i])) {
            return i;
        }
    }
    return -1;
}

PlayerAction CombatUI::GetActionAt(Vector2 point) const {
    int h = GetHoveredButton(point);
    if (h >= 0) return static_cast<PlayerAction>(h);
    return PlayerAction::NONE;
}

Rectangle CombatUI::GetButtonRect(int index) const {
    if (index >= 0 && index < 4) return buttons[index];
    return { 0.0f, 0.0f, 0.0f, 0.0f };
}
