#include "CombatPresentation.h"
#include <algorithm>
#include <cmath>

CombatPresentation::CombatPresentation() {
    Reset();
}

void CombatPresentation::Reset() {
    visualPlayerHp = 0.0f;
    visualPlayerMp = 0.0f;
    visualEnemyHp = 0.0f;
    heroLungeTimer = 0.0f;
    heroLungeOffset = { 0.0f, 0.0f };
    enemyLungeTimer = 0.0f;
    enemyLungeOffset = { 0.0f, 0.0f };
    enemyFlashTimer = 0.0f;
    playerFlashTimer = 0.0f;
    screenShakeTimer = 0.0f;
    screenShakeIntensity = 0.0f;
    shakeOffset = { 0.0f, 0.0f };
    skillEffectClass = -1;
    skillEffectTimer = 0.0f;
    healSparkleTimer = 0.0f;
    presentationTime = 0.0f;
    floatingTexts.clear();
}

void CombatPresentation::OnCombatStart(const Player& player, const Enemy& enemy, int mapCol) {
    // Determinacion del bioma segun la columna del mapa donde inicio el combate
    if (mapCol < 17) {
        biome = CombatBiome::VILLAGE;
    }
    else if (mapCol < 29) {
        biome = CombatBiome::MEADOW;
    }
    else if (mapCol < 40) {
        biome = CombatBiome::FOREST;
    }
    else {
        biome = CombatBiome::RUINS;
    }

    // Inicializacion inmediata de barras animadas para evitar animaciones desde 0
    visualPlayerHp = static_cast<float>(player.hp);
    visualPlayerMp = static_cast<float>(player.mp);
    visualEnemyHp = static_cast<float>(enemy.hp);

    heroLungeTimer = 0.0f;
    heroLungeOffset = { 0.0f, 0.0f };
    enemyLungeTimer = 0.0f;
    enemyLungeOffset = { 0.0f, 0.0f };
    enemyFlashTimer = 0.0f;
    playerFlashTimer = 0.0f;
    screenShakeTimer = 0.0f;
    screenShakeIntensity = 0.0f;
    shakeOffset = { 0.0f, 0.0f };
    skillEffectClass = -1;
    skillEffectTimer = 0.0f;
    healSparkleTimer = 0.0f;
    floatingTexts.clear();
}

void CombatPresentation::Update(float dt, const Player& player, const Enemy& enemy) {
    presentationTime += dt;

    // Interpolacion suave de barras hacia los valores reales del modelo
    const float hpSpeed = 160.0f;
    if (visualPlayerHp < player.hp) {
        visualPlayerHp = std::min(visualPlayerHp + hpSpeed * dt, static_cast<float>(player.hp));
    }
    else if (visualPlayerHp > player.hp) {
        visualPlayerHp = std::max(visualPlayerHp - hpSpeed * dt, static_cast<float>(player.hp));
    }

    if (visualEnemyHp < enemy.hp) {
        visualEnemyHp = std::min(visualEnemyHp + hpSpeed * dt, static_cast<float>(enemy.hp));
    }
    else if (visualEnemyHp > enemy.hp) {
        visualEnemyHp = std::max(visualEnemyHp - hpSpeed * dt, static_cast<float>(enemy.hp));
    }

    const float mpSpeed = 80.0f;
    if (visualPlayerMp < player.mp) {
        visualPlayerMp = std::min(visualPlayerMp + mpSpeed * dt, static_cast<float>(player.mp));
    }
    else if (visualPlayerMp > player.mp) {
        visualPlayerMp = std::max(visualPlayerMp - mpSpeed * dt, static_cast<float>(player.mp));
    }

    // Desplazamiento visual de atacantes (lunge / embestida)
    if (heroLungeTimer > 0.0f) {
        heroLungeTimer -= dt;
        if (heroLungeTimer <= 0.0f) {
            heroLungeTimer = 0.0f;
            heroLungeOffset = { 0.0f, 0.0f };
        }
        else {
            const float p = std::clamp(1.0f - (heroLungeTimer / 0.25f), 0.0f, 1.0f);
            heroLungeOffset.x = std::sin(p * 3.14159f) * 38.0f;
        }
    }
    else {
        heroLungeOffset = { 0.0f, 0.0f };
    }

    if (enemyLungeTimer > 0.0f) {
        enemyLungeTimer -= dt;
        if (enemyLungeTimer <= 0.0f) {
            enemyLungeTimer = 0.0f;
            enemyLungeOffset = { 0.0f, 0.0f };
        }
        else {
            const float p = std::clamp(1.0f - (enemyLungeTimer / 0.25f), 0.0f, 1.0f);
            enemyLungeOffset.x = -std::sin(p * 3.14159f) * 45.0f;
        }
    }
    else {
        enemyLungeOffset = { 0.0f, 0.0f };
    }

    // Destellos de impacto (hit flash)
    if (enemyFlashTimer > 0.0f) {
        enemyFlashTimer = std::max(0.0f, enemyFlashTimer - dt);
    }
    if (playerFlashTimer > 0.0f) {
        playerFlashTimer = std::max(0.0f, playerFlashTimer - dt);
    }

    // Sacudida de pantalla (screen shake)
    if (screenShakeTimer > 0.0f) {
        screenShakeTimer = std::max(0.0f, screenShakeTimer - dt);
        const float factor = std::clamp(screenShakeTimer / 0.3f, 0.0f, 1.0f);
        const float intensity = screenShakeIntensity * factor;
        shakeOffset.x = (static_cast<float>(GetRandomValue(-100, 100)) / 100.0f) * intensity;
        shakeOffset.y = (static_cast<float>(GetRandomValue(-100, 100)) / 100.0f) * intensity;
    }
    else {
        shakeOffset = { 0.0f, 0.0f };
    }

    // Temporizadores de efectos
    if (skillEffectTimer > 0.0f) {
        skillEffectTimer = std::max(0.0f, skillEffectTimer - dt);
    }
    if (healSparkleTimer > 0.0f) {
        healSparkleTimer = std::max(0.0f, healSparkleTimer - dt);
    }

    // Desplazamiento ascendente y desvanecimiento de textos flotantes
    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); ) {
        it->timer -= dt;
        it->pos.y -= 35.0f * dt;
        if (it->timer <= 0.0f) {
            it = floatingTexts.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CombatPresentation::TriggerPlayerAttack(int dmg) {
    heroLungeTimer = 0.25f;
    enemyFlashTimer = 0.25f;
    AddFloatingText("-" + std::to_string(dmg),
        { 740.0f + GetRandomValue(-20, 20), 180.0f },
        { 255, 90, 80, 255 }, 26);
}

void CombatPresentation::TriggerPlayerSkill(HeroClass heroClass, int dmg) {
    heroLungeTimer = 0.28f;
    enemyFlashTimer = 0.35f;
    skillEffectClass = static_cast<int>(heroClass);
    skillEffectTimer = 0.45f;
    TriggerScreenShake(0.32f, 7.5f);
    AddFloatingText("-" + std::to_string(dmg) + " CRITICO!",
        { 720.0f, 160.0f },
        { 255, 220, 80, 255 }, 28);
}

void CombatPresentation::TriggerPlayerHeal(int healedHp) {
    healSparkleTimer = 0.5f;
    AddFloatingText("+" + std::to_string(healedHp) + " HP",
        { 240.0f, 290.0f },
        { 100, 245, 130, 255 }, 26);
}

void CombatPresentation::TriggerEnemyAttack(bool isBoss, int dmg) {
    enemyLungeTimer = 0.25f;
    playerFlashTimer = 0.28f;
    if (isBoss || dmg >= 18) {
        TriggerScreenShake(0.25f, 5.0f);
    }
    AddFloatingText("-" + std::to_string(dmg),
        { 240.0f + GetRandomValue(-15, 15), 290.0f },
        { 255, 95, 85, 255 }, 26);
}

void CombatPresentation::TriggerScreenShake(float duration, float intensity) {
    screenShakeTimer = duration;
    screenShakeIntensity = intensity;
}

void CombatPresentation::AddFloatingText(const std::string& text, Vector2 pos, Color col, int fontSize) {
    FloatingCombatText ft;
    ft.text = text;
    ft.pos = pos;
    ft.color = col;
    ft.timer = 0.9f;
    ft.maxTimer = 0.9f;
    ft.fontSize = fontSize;
    floatingTexts.push_back(ft);
}
