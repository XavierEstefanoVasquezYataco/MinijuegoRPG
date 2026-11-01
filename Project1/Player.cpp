#include "Player.h"
#include <algorithm>
#include <cmath>

// ---------- Tabla de especies ----------
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
Player::Player() : Combatant("Heroe", 100, 30, 14, 5) {
    body.x = 2 * TileMap::TILE + 4;
    body.y = 8 * TileMap::TILE + 4;
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
    mp -= SKILL_COST;
    return true;
}

bool Player::UsePotion() {
    if (!CanUsePotion()) return false;
    --potions;
    Heal(POTION_HEAL);
    return true;
}

void Player::LevelUp() { ++level; maxHp += 15; maxMp += 5; atk += 3; def += 2; }
void Player::FullRestore() { hp = maxHp; mp = maxMp; }

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
    Vector2 c = Center();
    float dx = target.x - c.x, dy = target.y - c.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < detectRadius) ai = AIState::CHASE;
    else if (ai == AIState::CHASE && dist > loseRadius) ai = AIState::PATROL;

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
    map.Move(body, { dir.x * spd * dt, dir.y * spd * dt });
}
