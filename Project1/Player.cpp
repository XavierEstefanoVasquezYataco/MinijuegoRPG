#include "Player.h"
#include "PixelArt.h"
#include <algorithm>
#include <cmath>

// ---------- Tabla centralizada de arquetipos de héroes ----------
static const HeroClassData HERO_TABLE[3] = {
    {
        HeroClass::WARRIOR,
        "Guerrero",
        120, 15, 18, 6,
        180.0f,
        "Golpe Fuerte",
        10,
        2.0f,
        50, // 50% de probabilidad de huida
        "Mayor salud y resistencia física con impacto cuerpo a cuerpo demoledor.",
        "Ataque físico pesado con multiplicador x2.0",
        ART_WARRIOR,
        { 175, 185, 200, 255 }, { 235, 70, 70, 255 }
    },
    {
        HeroClass::MAGE,
        "Maga",
        80, 40, 22, 2,
        180.0f,
        "Centella Arcana",
        12,
        2.2f,
        50, // 50% de probabilidad de huida
        "Amplia reserva de mana y poderoso daño magico a costa de menor vitalidad.",
        "Descarga arcana concentrada con multiplicador x2.2",
        ART_MAGE,
        { 60, 80, 170, 255 }, { 170, 110, 240, 255 }
    },
    {
        HeroClass::SCOUT,
        "Exploradora",
        100, 25, 20, 4,
        207.0f, // +15% de velocidad en exploración
        "Tiro Critico",
        10,
        1.9f,
        65, // 65% de probabilidad de huida
        "Atributos equilibrados, paso veloz en el mapa y mayor facilidad para escapar.",
        "Disparo certero perforante con multiplicador x1.9",
        ART_SCOUT,
        { 55, 145, 75, 255 }, { 210, 170, 100, 255 }
    }
};

const HeroClassData& GetHeroClassData(HeroClass type) {
    return HERO_TABLE[static_cast<int>(type)];
}

// ---------- Tabla de especies de monstruos ----------
const std::vector<MonsterSpecies>& GetSpecies() {
    static const std::vector<MonsterSpecies> table = {
        // marca nombre        nv  hp  atk def boss  gate art  color cuerpo        color claro
        { 'A', "Slime",        1,  40, 10, 2, false, -1, 0,{ 80, 200, 90, 255 },{ 170, 240, 150, 255 } },
        { 'B', "Murcielago",   2,  55, 13, 3, false,  0, 1,{ 130, 90, 190, 255 },{ 190, 150, 240, 255 } },
        { 'C', "Goblin",       3,  70, 16, 4, false, -1, 2,{ 110, 160, 60, 255 },{ 170, 210, 100, 255 } },
        { 'D', "Lobo",         4,  90, 20, 5, false,  1, 3,{ 130, 130, 145, 255 },{ 190, 190, 205, 255 } },
        { 'E', "Esqueleto",    5, 105, 24, 6, false, -1, 2,{ 225, 225, 205, 255 },{ 250, 250, 240, 255 } },
        { 'F', "Dragon",       7, 160, 30, 8, true,  -1, 4,{ 200, 50, 60, 255 },{ 250, 130, 110, 255 } },
    };
    return table;
}

const MonsterSpecies* FindSpecies(char marker) {
    for (const auto& s : GetSpecies())
        if (s.marker == marker) return &s;
    return nullptr;
}

// ---------- Combatant ----------
Combatant::Combatant(const std::string& n, int hp_, int mp_, int atk_, int def_)
    : name(n), hp(hp_), maxHp(hp_), mp(mp_), maxMp(mp_), atk(atk_), def(def_) {
}

void Combatant::TakeDamage(int amount) { hp = std::max(0, hp - std::max(0, amount)); }
void Combatant::Heal(int amount) { hp = std::min(maxHp, hp + std::max(0, amount)); }

int Combatant::CalculateDamage(const Combatant& target, float multiplier) const {
    int variance = GetRandomValue(-2, 3);
    int dmg = static_cast<int>(atk * multiplier) - target.def + variance;
    return std::max(1, dmg);
}

// ---------- Player ----------
Player::Player() : Combatant("Guerrero", 120, 15, 18, 6) {
    body.x = 4.0f * TileMap::TILE + 4.0f;
    body.y = 14.0f * TileMap::TILE + 4.0f;
    InitClass(HeroClass::WARRIOR);
}

void Player::InitClass(HeroClass cls) {
    heroClass = cls;
    level = 1;
    potions = 3;
    const auto& data = GetHeroClassData(cls);
    name = data.name;
    maxHp = hp = data.hp;
    maxMp = mp = data.mp;
    atk = data.atk;
    def = data.def;
    speed = data.speed;
}

const char* Player::GetClassName() const {
    return GetHeroClassData(heroClass).name;
}

const char* Player::GetSkillName() const {
    return GetHeroClassData(heroClass).skillName;
}

int Player::GetSkillCost() const {
    return GetHeroClassData(heroClass).skillCost;
}

float Player::GetSkillMultiplier() const {
    return GetHeroClassData(heroClass).skillMultiplier;
}

int Player::GetFleeChance() const {
    return GetHeroClassData(heroClass).fleeChance;
}

Vector2 Player::GetDesiredMove(Vector2 inputDir, float dt) const {
    float len = std::sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (len > 0.0f) {
        return { (inputDir.x / len) * speed * dt, (inputDir.y / len) * speed * dt };
    }
    return { 0.0f, 0.0f };
}

void Player::Update(Vector2 inputDir, float dt, const TileMap& map) {
    Vector2 m = GetDesiredMove(inputDir, dt);
    moving = (m.x != 0.0f || m.y != 0.0f);
    if (moving) {
        if (std::fabs(m.x) > std::fabs(m.y)) facing = (m.x < 0) ? Facing::LEFT : Facing::RIGHT;
        else                                 facing = (m.y < 0) ? Facing::UP : Facing::DOWN;
        animTimer += dt;
    }
    else {
        animTimer = 0.0f;
    }
    map.Move(body, m);
}

bool Player::UseSkillCost() {
    if (!CanUseSkill()) return false;
    mp -= GetSkillCost();
    return true;
}

bool Player::UsePotion() {
    if (!CanUsePotion()) return false;
    --potions;
    Heal(POTION_HEAL);
    return true;
}

void Player::LevelUp() {
    ++level;
    maxHp += 15;
    maxMp += 5;
    atk += 3;
    def += 2;
}

void Player::FullRestore() {
    hp = maxHp;
    mp = maxMp;
}

// ---------- Enemy ----------
Enemy::Enemy(const MonsterSpecies& sp, int col, int row)
    : Combatant(sp.name, sp.hp, 0, sp.atk, sp.def) {
    level = sp.level;
    isBoss = sp.boss;
    unlocksWall = sp.gate;
    art = sp.art;
    bodyColor = sp.body;
    lightColor = sp.light;
    body.x = col * TileMap::TILE + 4.0f;
    body.y = row * TileMap::TILE + 4.0f;
    home = Center();
    if (isBoss) { detectRadius = 160.0f; chaseSpeed = 85.0f; }
}

void Enemy::UpdateAI(float dt, Vector2 target, const TileMap& map) {
    if (defeated) return;

    // Los enemigos nunca pueden entrar a la aldea segura (col < 17)
    constexpr float VILLAGE_LIMIT_X = 17.0f * TileMap::TILE;

    Vector2 c = Center();
    float dx = target.x - c.x, dy = target.y - c.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Si el jugador se resguarda dentro de la aldea segura, se interrumpe la persecucion
    if (target.x < VILLAGE_LIMIT_X) {
        ai = AIState::PATROL;
    } else {
        if (dist < detectRadius) ai = AIState::CHASE;
        else if (ai == AIState::CHASE && dist > loseRadius) ai = AIState::PATROL;
    }

    Vector2 dir{ 0, 0 };
    float spd = 0;

    switch (ai) {
    case AIState::CHASE:
        if (dist > 0) dir = { dx / dist, dy / dist };
        spd = chaseSpeed;
        break;
    case AIState::PATROL: {
        patrolTimer -= dt;
        if (patrolTimer <= 0) {
            patrolTimer = GetRandomValue(10, 25) / 10.0f;
            static const Vector2 dirs[5] = { { 0,0 },{ 1,0 },{ -1,0 },{ 0,1 },{ 0,-1 } };
            patrolDir = dirs[GetRandomValue(0, 4)];
        }
        float hx = home.x - c.x, hy = home.y - c.y;
        float hd = std::sqrt(hx * hx + hy * hy);
        if (hd > 96.0f) patrolDir = { hx / hd, hy / hd };
        dir = patrolDir;
        spd = patrolSpeed;
        break;
    }
    }

    Vector2 delta = { dir.x * spd * dt, dir.y * spd * dt };
    // Evita explicitamente que un enemigo cruce fisicamente hacia la aldea
    if (body.x + delta.x < VILLAGE_LIMIT_X) {
        delta.x = 0.0f;
    }

    map.Move(body, delta);
}
