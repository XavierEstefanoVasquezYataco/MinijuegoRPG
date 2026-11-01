#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "Player.h"

// Biomas posibles del escenario de combate segun la zona de inicio
enum class CombatBiome {
    VILLAGE,
    MEADOW,
    FOREST,
    RUINS
};

// Texto flotante de dano y sanacion
struct FloatingCombatText {
    std::string text;
    Vector2 pos{ 0.0f, 0.0f };
    Color color{ 255, 255, 255, 255 };
    float timer = 0.9f;
    float maxTimer = 0.9f;
    int fontSize = 24;
};

// Modulo de presentacion visual del combate:
// Gestiona interpolacion de barras, animaciones de embestida (lunge),
// sacudida de escenario, destellos y efectos de particulas.
// No altera reglas de combate, vida real ni transiciones del modelo.
class CombatPresentation {
public:
    CombatPresentation();

    void Reset();
    void OnCombatStart(const Player& player, const Enemy& enemy, int mapCol);
    void Update(float dt, const Player& player, const Enemy& enemy);

    // Disparadores visuales vinculados a resultados logicos
    void TriggerPlayerAttack(int dmg);
    void TriggerPlayerSkill(HeroClass heroClass, int dmg);
    void TriggerPlayerHeal(int healedHp);
    void TriggerEnemyAttack(bool isBoss, int dmg);
    void TriggerScreenShake(float duration, float intensity);

    // Consultas para renderizado en CombatUI
    CombatBiome GetBiome() const { return biome; }
    float GetVisualPlayerHp() const { return visualPlayerHp; }
    float GetVisualPlayerMp() const { return visualPlayerMp; }
    float GetVisualEnemyHp() const { return visualEnemyHp; }
    Vector2 GetHeroLungeOffset() const { return heroLungeOffset; }
    Vector2 GetEnemyLungeOffset() const { return enemyLungeOffset; }
    Vector2 GetShakeOffset() const { return shakeOffset; }
    float GetEnemyFlash() const { return enemyFlashTimer; }
    float GetPlayerFlash() const { return playerFlashTimer; }
    float GetSkillTimer() const { return skillEffectTimer; }
    int GetSkillClass() const { return skillEffectClass; }
    float GetHealSparkleTimer() const { return healSparkleTimer; }
    float GetPresentationTime() const { return presentationTime; }
    const std::vector<FloatingCombatText>& GetFloatingTexts() const { return floatingTexts; }

private:
    CombatBiome biome = CombatBiome::MEADOW;

    // Barras interpoladas
    float visualPlayerHp = 0.0f;
    float visualPlayerMp = 0.0f;
    float visualEnemyHp = 0.0f;

    // Desplazamiento visual de atacantes (lunge)
    float heroLungeTimer = 0.0f;
    Vector2 heroLungeOffset{ 0.0f, 0.0f };
    float enemyLungeTimer = 0.0f;
    Vector2 enemyLungeOffset{ 0.0f, 0.0f };

    // Destellos de impacto (hit flash)
    float enemyFlashTimer = 0.0f;
    float playerFlashTimer = 0.0f;

    // Sacudida de pantalla leve
    float screenShakeTimer = 0.0f;
    float screenShakeIntensity = 0.0f;
    Vector2 shakeOffset{ 0.0f, 0.0f };

    // Efectos de habilidades y pocion
    int skillEffectClass = -1;
    float skillEffectTimer = 0.0f;
    float healSparkleTimer = 0.0f;

    // Reloj interno de presentacion (se congela durante la pausa)
    float presentationTime = 0.0f;

    // Textos flotantes
    std::vector<FloatingCombatText> floatingTexts;

    void AddFloatingText(const std::string& text, Vector2 pos, Color col, int fontSize = 24);
};
