/**
 * @file CombatPresentation.h
 * @brief Módulo de presentación visual, animaciones y efectos procedimentales de combate.
 *
 * Capa de Arquitectura: Vista (View) - Módulo Especializado de Presentación y Efectos.
 *
 * Propósito y Responsabilidades:
 * - Gestionar la interpolación fluida de barras de salud (HP) y maná (MP) desacoplada de la lógica del modelo.
 * - Administrar las animaciones de embestida (*lunge*) de atacantes hacia su objetivo con retorno elástico.
 * - Coordinar destellos de daño (*hit flash*), sacudida de pantalla (*screen shake*) y partículas de habilidad.
 * - Gestionar la física y ciclo de vida de los números flotantes de combate (daño recibido y vida curada).
 * - Proporcionar un reloj interno de presentación (`presentationTime`) que se congela durante la pausa
 *   para garantizar que no existan saltos temporales ni desincronizaciones al reanudar la partida.
 *
 * Relaciones Principales:
 * - Actualizado por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) al cambiar estados de combate o registrar impactos.
 * - Consultado por [Vistas/CombatUI.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatUI.h) para obtener desfases de posición, opacidades y valores visuales de barras.
 * - Lee datos inmutables de [Modelo/Player.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/Player.h).
 *
 * Observaciones Arquitectónicas:
 * - No modifica en ningún momento la salud real, el maná ni el inventario de las entidades.
 *   Si la presentación se desactivase por completo, las reglas de combate seguirían siendo 100% funcionales.
 */

#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "../Modelo/Player.h"

/**
 * @enum CombatBiome
 * @brief Bioma visual de la arena de combate seleccionado según la coordenada del mapa donde comenzó la batalla.
 *
 * - VILLAGE: Colinas suaves y pradera domesticada (columna < 17).
 * - MEADOW: Pradera abierta con cielo celeste y flores silvestres (columna 17..28).
 * - FOREST: Bosque crepuscular, esporas flotantes y suelo musgoso (columna 29..39).
 * - RUINS: Cubil de ruinas ancestrales con columnas derruidas, cielo rojizo y brasas ascendentes (columna >= 40).
 */
enum class CombatBiome {
    VILLAGE,
    MEADOW,
    FOREST,
    RUINS
};

/**
 * @struct FloatingCombatText
 * @brief Texto numérico ascendente con desvanecimiento alfa para ilustrar impactos y curaciones.
 */
struct FloatingCombatText {
    std::string text;                       ///< Texto mostrado (ej. "-15", "-42 CRITICO!", "+40 HP").
    Vector2 pos{ 0.0f, 0.0f };               ///< Posición en pantalla que asciende con la velocidad vertical.
    Color color{ 255, 255, 255, 255 };      ///< Tono semántico (rojo para daño, dorado para crítico, verde para curación).
    float timer = 0.9f;                     ///< Tiempo de vida restante en segundos.
    float maxTimer = 0.9f;                  ///< Tiempo de vida total (usado para calcular la opacidad alfa).
    int fontSize = 24;                      ///< Tamaño de fuente tipográfico.
};

/**
 * @class CombatPresentation
 * @brief Orquestador de efectos visuales y animaciones procedimentales de combate.
 */
class CombatPresentation {
public:
    CombatPresentation();

    /**
     * @brief Restablece a cero todos los desplazamientos, destellos, barras y listas de partículas.
     */
    void Reset();

    /**
     * @brief Inicializa el módulo al comenzar un combate, determinando el bioma e igualando las barras.
     *
     * @param player Datos del héroe para calibrar barras de HP/MP iniciales sin animación desde 0.
     * @param enemy Datos del enemigo para calibrar su barra de salud.
     * @param mapCol Columna de la casilla del mapa donde se produjo el contacto.
     */
    void OnCombatStart(const Player& player, const Enemy& enemy, int mapCol);

    /**
     * @brief Actualiza los temporizadores de animación, la interpolación de barras y la física de textos flotantes.
     *
     * Solo debe ejecutarse cuando el juego no esté en pausa para mantener el reloj congelado.
     *
     * @param dt Tiempo transcurrido en el fotograma.
     * @param player Referencia constante al jugador para conocer su salud/maná objetivo.
     * @param enemy Referencia constante al enemigo para conocer su salud objetivo.
     */
    void Update(float dt, const Player& player, const Enemy& enemy);

    // Disparadores visuales vinculados a eventos del controlador
    void TriggerPlayerAttack(int dmg);
    void TriggerPlayerSkill(HeroClass heroClass, int dmg);
    void TriggerPlayerHeal(int healedHp);
    void TriggerEnemyAttack(bool isBoss, int dmg);
    void TriggerScreenShake(float duration, float intensity);

    // Consultas de solo lectura para CombatUI
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

    // Embestida visual (lunge)
    float heroLungeTimer = 0.0f;
    Vector2 heroLungeOffset{ 0.0f, 0.0f };
    float enemyLungeTimer = 0.0f;
    Vector2 enemyLungeOffset{ 0.0f, 0.0f };

    // Destellos de impacto (hit flash)
    float enemyFlashTimer = 0.0f;
    float playerFlashTimer = 0.0f;

    // Sacudida de pantalla
    float screenShakeTimer = 0.0f;
    float screenShakeIntensity = 0.0f;
    Vector2 shakeOffset{ 0.0f, 0.0f };

    // Efectos de habilidades y pociones
    int skillEffectClass = -1;
    float skillEffectTimer = 0.0f;
    float healSparkleTimer = 0.0f;

    // Reloj interno inmune a saltos por pausa
    float presentationTime = 0.0f;

    // Contenedor de textos emergentes
    std::vector<FloatingCombatText> floatingTexts;

    void AddFloatingText(const std::string& text, Vector2 pos, Color col, int fontSize = 24);
};
