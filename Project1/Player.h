#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "GameState.h"
#include "TileMap.h"

// Datos de presentacion y atributos base de cada clase de heroe (tabla centralizada)
struct HeroClassData {
    HeroClass type;
    const char* name;
    int hp, mp, atk, def;
    float speed;
    const char* skillName;
    int skillCost;
    float skillMultiplier;
    int fleeChance;         // 50% Guerrero/Maga, 65% Exploradora
    const char* description;
    const char* skillEffect;
    int artId;
    Color bodyColor;
    Color lightColor;
};

const HeroClassData& GetHeroClassData(HeroClass type);

// Datos de cada especie de monstruo (se agregan en Player.cpp)
struct MonsterSpecies {
    char marker;            // letra en el mapa
    const char* name;
    int level;
    int hp, atk, def;
    bool boss;
    int gate;               // compuerta que abre al ser vencido (-1 = ninguna)
    int art;                // 0 blob, 1 murcielago, 2 humanoide, 3 bestia, 4 dragon
    Color body, light;
    int expReward;          // Puntos de experiencia otorgados al ser derrotado
};

const std::vector<MonsterSpecies>& GetSpecies();
const MonsterSpecies* FindSpecies(char marker);

class Combatant {
public:
    std::string name;
    int level = 1;
    int hp = 1, maxHp = 1;
    int mp = 0, maxMp = 0;
    int atk = 1, def = 0;

    Combatant() = default;
    Combatant(const std::string& n, int hp_, int mp_, int atk_, int def_);
    virtual ~Combatant() = default;

    bool IsAlive() const { return hp > 0; }
    void TakeDamage(int amount);
    void Heal(int amount);
    int CalculateDamage(const Combatant& target, float multiplier = 1.0f) const;
};

struct StatGains {
    int maxHp = 0;
    int maxMp = 0;
    int atk = 0;
    int def = 0;
};

struct ExpResult {
    bool leveledUp = false;
    int levelsGained = 0;
    StatGains totalGains;
};

class Player : public Combatant {
public:
    HeroClass heroClass = HeroClass::WARRIOR;
    Rectangle body{ 0, 0, 24, 24 };
    float speed = 180.0f;
    int potions = 3;

    int exp = 0;
    int expNext = 50;

    Facing facing = Facing::DOWN;
    bool moving = false;
    float animTimer = 0.0f;

    static constexpr int POTION_HEAL = 40;

    Player();
    void InitClass(HeroClass cls);

    const char* GetClassName() const;
    const char* GetSkillName() const;
    int GetSkillCost() const;
    float GetSkillMultiplier() const;
    int GetFleeChance() const;

    Vector2 GetDesiredMove(Vector2 inputDir, float dt) const;
    void Update(Vector2 inputDir, float dt, const TileMap& map);
    Vector2 Center() const { return { body.x + body.width / 2, body.y + body.height / 2 }; }

    bool CanUseSkill() const { return mp >= GetSkillCost(); }
    bool UseSkillCost();
    bool CanUsePotion() const { return potions > 0 && hp < maxHp; }
    int UsePotion(); // Retorna HP efectivamente curado (0 si ya estaba lleno o no habia)

    ExpResult GainExp(int amount);
    StatGains LevelUp();
    void FullRestore();
};

class Enemy : public Combatant {
public:
    Rectangle body{ 0, 0, 24, 24 };
    bool defeated = false;
    bool isBoss = false;
    int unlocksWall = -1;
    int expReward = 0;

    int art = 0;
    Color bodyColor{ 255, 255, 255, 255 };
    Color lightColor{ 255, 255, 255, 255 };

    AIState ai = AIState::PATROL;
    Vector2 home{ 0, 0 };
    Vector2 patrolDir{ 0, 0 };
    float patrolTimer = 0.0f;
    float patrolSpeed = 40.0f;
    float chaseSpeed = 75.0f;
    float detectRadius = 130.0f;
    float loseRadius = 220.0f;

    // Se crea a partir de una especie y la posicion (tile) encontrada en el mapa
    Enemy(const MonsterSpecies& sp, int col, int row);

    Vector2 Center() const { return { body.x + body.width / 2, body.y + body.height / 2 }; }
    void UpdateAI(float dt, Vector2 target, const TileMap& map);
};
