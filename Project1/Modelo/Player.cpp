/**
 * @file Player.cpp
 * @brief Implementación de las clases de combate (Combatant, Player, Enemy) y tablas de arquetipos.
 *
 * Capa de Arquitectura: Modelo (Model) - Lógica de Combate y Comportamiento de Entidades.
 *
 * Propósito y Responsabilidades:
 * - Implementar las tablas maestras de arquetipos de héroe (`HERO_TABLE`) y especies de monstruos.
 * - Proveer la matemática de combate (fórmulas de daño con factor aleatorio de dispersión).
 * - Implementar el sistema de experiencia acumulativa multinivel (`GainExp`) y persistencia estricta.
 * - Implementar la máquina de estados de IA para enemigos (patrulla aleatoria, persecución y frontera segura).
 *
 * Observaciones Arquitectónicas sobre Responsabilidades Mezcladas:
 * - Este archivo incluye `../Vistas/PixelArt.h` para acceder a los identificadores `ART_WARRIOR`,
 *   `ART_MAGE`, `ART_SCOUT`. Esta referencia vincula constantes de presentación dentro de los datos
 *   de inicialización del modelo, lo cual se documenta aquí como observación arquitectónica pedagógica.
 */

#include "Player.h"
#include "../Vistas/PixelArt.h"
#include <algorithm>
#include <cmath>

// ---------- Tabla centralizada de arquetipos de héroes ----------

/**
 * @brief Tabla inmutable con las propiedades de los tres héroes disponibles.
 *
 * Balance de arquetipos:
 * 1. Guerrero: Tanque / daño físico pesado (120 HP, 15 MP, 18 ATK, 6 DEF, vel 180, huida 50%).
 * 2. Maga: Cañón de cristal mágico (80 HP, 40 MP, 22 ATK, 2 DEF, vel 180, huida 50%).
 * 3. Exploradora: Ágil y equilibrada (100 HP, 25 MP, 20 ATK, 4 DEF, vel 207 [+15%], huida 65%).
 */
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

/**
 * @brief Obtiene la información inmutable asociada al arquetipo solicitado.
 * @param type Enumeración de clase (WARRIOR = 0, MAGE = 1, SCOUT = 2).
 * @return const HeroClassData& Referencia a la entrada en HERO_TABLE.
 */
const HeroClassData& GetHeroClassData(HeroClass type) {
    return HERO_TABLE[static_cast<int>(type)];
}

// ---------- Tabla de especies de monstruos ----------

/**
 * @brief Definición de todas las especies de enemigos distribuidas en los biomas.
 *
 * - 'A' Slime: Nivel 1 en Pradera (40 HP, 10 ATK, 2 DEF, 25 EXP).
 * - 'B' Murciélago: Nivel 2 en Pradera (55 HP, 13 ATK, 3 DEF, abre Compuerta 1, 50 EXP).
 * - 'C' Goblin: Nivel 3 en Bosque (70 HP, 16 ATK, 4 DEF, 60 EXP).
 * - 'D' Lobo: Nivel 4 en Bosque (90 HP, 20 ATK, 5 DEF, abre Compuerta 2, 90 EXP).
 * - 'E' Esqueleto: Nivel 5 en Ruinas (105 HP, 24 ATK, 6 DEF, 110 EXP).
 * - 'F' Dragón: Nivel 7 en Sanctum de Ruinas (160 HP, 30 ATK, 8 DEF, Jefe Final, 300 EXP).
 */
const std::vector<MonsterSpecies>& GetSpecies() {
    static const std::vector<MonsterSpecies> table = {
        // marca nombre        nv  hp  atk def boss  gate art  color cuerpo          color claro           exp
        { 'A', "Slime",        1,  40, 10, 2, false, -1, 0, { 80, 200, 90, 255 },  { 170, 240, 150, 255 }, 25 },
        { 'B', "Murcielago",   2,  55, 13, 3, false,  0, 1, { 130, 90, 190, 255 }, { 190, 150, 240, 255 }, 50 },
        { 'C', "Goblin",       3,  70, 16, 4, false, -1, 2, { 110, 160, 60, 255 }, { 170, 210, 100, 255 }, 60 },
        { 'D', "Lobo",         4,  90, 20, 5, false,  1, 3, { 130, 130, 145, 255 },{ 190, 190, 205, 255 }, 90 },
        { 'E', "Esqueleto",    5, 105, 24, 6, false, -1, 2, { 225, 225, 205, 255 },{ 250, 250, 240, 255 }, 110 },
        { 'F', "Dragon",       7, 160, 30, 8, true,  -1, 4, { 200, 50, 60, 255 },  { 250, 130, 110, 255 }, 300 },
    };
    return table;
}

/**
 * @brief Busca una especie en la tabla de monstruos por su letra identificadora en el mapa.
 * @param marker Carácter 'A'..'F'.
 * @return const MonsterSpecies* Puntero al elemento de la tabla o nullptr si no se encuentra.
 */
const MonsterSpecies* FindSpecies(char marker) {
    for (const auto& s : GetSpecies()) {
        if (s.marker == marker) return &s;
    }
    return nullptr;
}

// ---------- Combatant ----------

/**
 * @brief Inicializa un combatiente con sus valores numéricos completos.
 */
Combatant::Combatant(const std::string& n, int hp_, int mp_, int atk_, int def_)
    : name(n), hp(hp_), maxHp(hp_), mp(mp_), maxMp(mp_), atk(atk_), def(def_) {
}

/**
 * @brief Aplica daño a la salud actual, garantizando que nunca descienda por debajo de 0.
 * @param amount Puntos de daño recibidos.
 */
void Combatant::TakeDamage(int amount) {
    hp = std::max(0, hp - std::max(0, amount));
}

/**
 * @brief Restaura puntos de salud respetando el tope máximo de `maxHp`.
 * @param amount Puntos curados.
 */
void Combatant::Heal(int amount) {
    hp = std::min(maxHp, hp + std::max(0, amount));
}

/**
 * @brief Calcula el daño saliente aplicando mitigación por defensa y varianza pseudoaleatoria.
 *
 * Algoritmo:
 * 1. Genera una varianza aleatoria entera entre -2 y +3 mediante `GetRandomValue(-2, 3)`.
 * 2. Aplica la fórmula: `daño = static_cast<int>(atk * multiplier) - target.def + variance`.
 * 3. Utiliza `std::max(1, daño)` para asegurar que todo impacto acertado cause como mínimo 1 pt de daño.
 *
 * Ejemplo numérico 1 (Ataque básico del Guerrero contra Slime):
 * - Atacante: Guerrero con atk = 18, multiplier = 1.0f.
 * - Defensor: Slime con target.def = 2.
 * - Supongamos que GetRandomValue(-2, 3) genera +1.
 * - Operación: static_cast<int>(18 * 1.0f) = 18.
 * - Daño bruto mitigado: 18 - 2 + 1 = 17.
 * - std::max(1, 17) -> Resultado final: 17 puntos de vida (HP) reducidos al objetivo.
 *
 * Ejemplo numérico 2 (Técnica "Golpe Fuerte" con multiplicador x2.0 contra Dragón):
 * - Atacante: Guerrero con atk = 18, multiplier = 2.0f.
 * - Defensor: Dragón con target.def = 8.
 * - Supongamos que GetRandomValue(-2, 3) genera -2.
 * - Operación: static_cast<int>(18 * 2.0f) = 36.
 * - Daño bruto mitigado: 36 - 8 + (-2) = 26.
 * - std::max(1, 26) -> Resultado final: 26 puntos de vida (HP) reducidos al objetivo.
 *
 * @param target Entidad defensora cuya armadura mitigará el daño.
 * @param multiplier Coeficiente multiplicador sobre ATK (1.0f ataque normal, >1.0f habilidad).
 * @return int Daño neto que recibirá el defensor (garantizado >= 1).
 */
int Combatant::CalculateDamage(const Combatant& target, float multiplier) const {
    int variance = GetRandomValue(-2, 3);
    int dmg = static_cast<int>(atk * multiplier) - target.def + variance;
    return std::max(1, dmg);
}

// ---------- Player ----------

/**
 * @brief Constructor por defecto del héroe.
 *
 * Sitúa al jugador en la Aldea Segura (columna 4, fila 14 del TileMap)
 * e inicializa el arquetipo predeterminado (Guerrero).
 */
Player::Player() : Combatant("Guerrero", 120, 15, 18, 6) {
    body.x = 4.0f * TileMap::TILE + 4.0f;
    body.y = 14.0f * TileMap::TILE + 4.0f;
    InitClass(HeroClass::WARRIOR);
}

/**
 * @brief Inicializa las estadísticas completas del jugador a nivel 1 según la clase elegida.
 * @param cls Clase seleccionada (WARRIOR, MAGE, SCOUT).
 */
void Player::InitClass(HeroClass cls) {
    heroClass = cls;
    level = 1;
    exp = 0;
    expNext = 50;
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

/**
 * @brief Calcula el desplazamiento deseado normalizando la dirección de entrada.
 *
 * Evita el problema clásico de velocidad diagonal incrementada multiplicando
 * por el inverso de la norma euclidiana cuando la longitud es mayor a cero.
 *
 * Ejemplo numérico de normalización vectorial diagonal:
 * - Entrada: inputDir = { 1.0f, 1.0f } (jugador presionando 'D' y 'S' simultáneamente).
 * - Longitud euclidiana: len = sqrt(1.0^2 + 1.0^2) = sqrt(2) ≈ 1.4142f.
 * - Factor normalizado: (1.0f / 1.4142f) ≈ 0.7071f.
 * - Suponiendo velocidad del Guerrero (speed = 180.0 px/s) a 60 FPS (dt ≈ 0.01666s):
 *   Desplazamiento X: 0.7071f * 180.0f * 0.01666s ≈ 2.12 píxeles.
 *   Desplazamiento Y: 0.7071f * 180.0f * 0.01666s ≈ 2.12 píxeles.
 * - Magnitud resultante: sqrt(2.12^2 + 2.12^2) ≈ 3.00 píxeles por fotograma.
 * - Sin normalización: avanzaría 3.00 px en X y 3.00 px en Y, resultando en
 *   sqrt(3^2 + 3^2) ≈ 4.24 píxeles (un 41.4% más rápido en diagonal).
 *
 * @param inputDir Vector con los componentes direccionales (-1, 0, 1).
 * @param dt Tiempo transcurrido del fotograma en segundos.
 * @return Vector2 Desplazamiento proporcional a `speed * dt` en píxeles.
 */
Vector2 Player::GetDesiredMove(Vector2 inputDir, float dt) const {
    float len = std::sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (len > 0.0f) {
        return { (inputDir.x / len) * speed * dt, (inputDir.y / len) * speed * dt };
    }
    return { 0.0f, 0.0f };
}

/**
 * @brief Actualiza la orientación del sprite y delega el movimiento con colisiones a TileMap.
 *
 * Determina si el movimiento horizontal predomina sobre el vertical para fijar la orientación
 * de la mirada (`facing`) y acumula tiempo en `animTimer` para alternar los cuadros de animación.
 *
 * @param inputDir Dirección proveniente del teclado.
 * @param dt Tiempo del fotograma.
 * @param map Cuadrícula del mundo para verificar colisiones sólidas.
 */
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

/**
 * @brief Consume los puntos de maná correspondientes al coste de la técnica especial.
 * @return true Si se pudo descontar; false si no había suficiente maná.
 */
bool Player::UseSkillCost() {
    if (!CanUseSkill()) return false;
    mp -= GetSkillCost();
    return true;
}

/**
 * @brief Aplica el uso inteligente de poción.
 *
 * Reglas de diseño:
 * - Si no quedan pociones o la salud ya está al máximo, retorna 0 y no gasta turno.
 * - Si se consume, descuenta 1 poción y restaura hasta 40 HP (sin exceder `maxHp`).
 *
 * @return int Puntos de vida efectivamente curados (permite reportar el valor exacto en el log).
 */
int Player::UsePotion() {
    if (potions <= 0 || hp >= maxHp) {
        return 0;
    }
    --potions;
    const int prevHp = hp;
    Heal(POTION_HEAL);
    return hp - prevHp;
}

/**
 * @brief Acumula puntos de experiencia y procesa subidas de nivel continuas mediante un bucle while.
 *
 * Fórmula del umbral: `expNext = level * 50` (en puntos de EXP).
 * Si una recompensa grande otorga suficiente experiencia para varios niveles (por ejemplo, vencer
 * al Dragón final que otorga 300 EXP), el bucle `while (exp >= expNext)` consume los umbrales de manera sucesiva
 * y acumula las mejoras de atributos en la estructura `ExpResult`.
 *
 * Ejemplo numérico de subida multinivel continua (+300 EXP del Dragón):
 * - Estado inicial: Nivel 1, exp = 0, expNext = 1 * 50 = 50 EXP.
 * - Ingreso: amount = 300 EXP -> exp pasa a ser 300.
 * - Iteración 1: exp (300) >= expNext (50) -> exp = 300 - 50 = 250.
 *   Se invoca LevelUp() -> sube a Nivel 2. Nuevo umbral: expNext = 2 * 50 = 100 EXP.
 * - Iteración 2: exp (250) >= expNext (100) -> exp = 250 - 100 = 150.
 *   Se invoca LevelUp() -> sube a Nivel 3. Nuevo umbral: expNext = 3 * 50 = 150 EXP.
 * - Iteración 3: exp (150) >= expNext (150) -> exp = 150 - 150 = 0.
 *   Se invoca LevelUp() -> sube a Nivel 4. Nuevo umbral: expNext = 4 * 50 = 200 EXP.
 * - Condición de salida: exp (0) < expNext (200) -> Fin del bucle.
 * - Resultado reportado: leveledUp = true, levelsGained = 3, exp = 0, expNext = 200,
 *   con la suma acumulada de las estadísticas de los 3 niveles en totalGains.
 *
 * @param amount Cantidad de puntos de experiencia a ingresar.
 * @return ExpResult Resumen detallado de niveles subidos y aumentos totales de atributos.
 */
ExpResult Player::GainExp(int amount) {
    ExpResult res;
    if (amount <= 0) return res;

    exp += amount;
    while (exp >= expNext) {
        exp -= expNext;
        const StatGains g = LevelUp();
        res.leveledUp = true;
        res.levelsGained++;
        res.totalGains.maxHp += g.maxHp;
        res.totalGains.maxMp += g.maxMp;
        res.totalGains.atk += g.atk;
        res.totalGains.def += g.def;
        expNext = level * 50;
    }
    return res;
}

/**
 * @brief Incrementa el nivel del héroe y amplía sus topes de atributos según su clase.
 *
 * Tabla de ganancias por nivel:
 * - Guerrero: +20 HP max, +3 MP max, +4 ATK, +3 DEF.
 * - Maga: +10 HP max, +10 MP max, +5 ATK, +1 DEF.
 * - Exploradora: +15 HP max, +6 MP max, +4 ATK, +2 DEF.
 *
 * Nota pedagógica: Los valores de `hp` y `mp` actuales NO se modifican,
 * reforzando la persistencia estricta de recursos entre batallas.
 *
 * @return StatGains Atributos añadidos en este nivel específico.
 */
StatGains Player::LevelUp() {
    ++level;
    StatGains g;
    switch (heroClass) {
    case HeroClass::WARRIOR:
        g = { 20, 3, 4, 3 };
        break;
    case HeroClass::MAGE:
        g = { 10, 10, 5, 1 };
        break;
    case HeroClass::SCOUT:
    default:
        g = { 15, 6, 4, 2 };
        break;
    }

    maxHp += g.maxHp;
    maxMp += g.maxMp;
    atk += g.atk;
    def += g.def;

    return g;
}

/**
 * @brief Restaura totalmente la salud y el maná a sus valores máximos.
 *
 * Utilizada exclusivamente por la Curandera Elena en la aldea.
 */
void Player::FullRestore() {
    hp = maxHp;
    mp = maxMp;
}

// ---------- Enemy ----------

/**
 * @brief Construye una instancia de enemigo a partir de los datos de su especie y posición en el mapa.
 *
 * Configura el ancla inicial (`home`) y asigna parámetros especiales si es un jefe (Dragón):
 * mayor radio de detección (160 px) y mayor velocidad de persecución (85 px/s).
 *
 * @param sp Registro de la especie de monstruo.
 * @param col Columna del tile de origen.
 * @param row Fila del tile de origen.
 */
Enemy::Enemy(const MonsterSpecies& sp, int col, int row)
    : Combatant(sp.name, sp.hp, 0, sp.atk, sp.def),
      art(sp.art),
      bodyColor(sp.body),
      lightColor(sp.light),
      isBoss(sp.boss),
      unlocksWall(sp.gate),
      expReward(sp.expReward) {
    level = sp.level;
    body.x = col * TileMap::TILE + 4.0f;
    body.y = row * TileMap::TILE + 4.0f;
    home = Center();
    if (isBoss) {
        detectRadius = 160.0f;
        chaseSpeed = 85.0f;
    }
}

/**
 * @brief Máquina de estados de la Inteligencia Artificial del enemigo en el supramundo.
 *
 * Lógica pedagógica de comportamiento:
 * 1. Comprobación de aldea segura: Si el héroe se encuentra en la Aldea Segura (coordenada X < 17 * TILE),
 *    el monstruo cancela cualquier persecución y regresa inmediatamente a PATROL.
 * 2. Transición a persecución (CHASE): Si el héroe está fuera de la aldea y la distancia euclidiana
 *    es menor a `detectRadius` (130 px), el enemigo enfoca su vector hacia el jugador.
 * 3. Pérdida de objetivo: Si el enemigo está en CHASE pero el jugador se aleja más allá de `loseRadius` (220 px),
 *    la criatura desiste y vuelve al estado PATROL.
 * 4. Patrulla aleatoria y anclaje: En PATROL, cada 1.0 a 2.5 segundos elige una dirección cardinal aleatoria.
 *    Si se aleja más de 96 píxeles de su punto de generación (`home`), fuerza su rumbo de regreso a casa.
 * 5. Barrera física de seguridad: Se restringe expresamente que `body.x + delta.x` ingrese a la Aldea Segura.
 *
 * @param dt Tiempo transcurrido del fotograma en segundos.
 * @param target Coordenadas centrales del jugador.
 * @param map Cuadrícula del mapa para resolución de colisiones sólidas.
 */
void Enemy::UpdateAI(float dt, Vector2 target, const TileMap& map) {
    if (defeated) return;

    // Los enemigos nunca pueden entrar a la aldea segura (col < 17)
    constexpr float VILLAGE_LIMIT_X = 17.0f * TileMap::TILE;

    Vector2 c = Center();
    float dx = target.x - c.x, dy = target.y - c.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Si el jugador se resguarda dentro de la aldea segura, se interrumpe la persecución
    if (target.x < VILLAGE_LIMIT_X) {
        ai = AIState::PATROL;
    } else {
        if (dist < detectRadius) {
            ai = AIState::CHASE;
        } else if (ai == AIState::CHASE && dist > loseRadius) {
            ai = AIState::PATROL;
        }
    }

    Vector2 dir{ 0.0f, 0.0f };
    float spd = 0.0f;

    switch (ai) {
    case AIState::CHASE:
        // Vector normalizado apuntando directamente al jugador
        if (dist > 0.0f) dir = { dx / dist, dy / dist };
        spd = chaseSpeed;
        break;

    case AIState::PATROL: {
        patrolTimer -= dt;
        if (patrolTimer <= 0.0f) {
            patrolTimer = GetRandomValue(10, 25) / 10.0f;
            static const Vector2 dirs[5] = { { 0,0 }, { 1,0 }, { -1,0 }, { 0,1 }, { 0,-1 } };
            patrolDir = dirs[GetRandomValue(0, 4)];
        }
        // Corrección de anclaje: si se aleja demasiado de home, regresa a su origen
        float hx = home.x - c.x, hy = home.y - c.y;
        float hd = std::sqrt(hx * hx + hy * hy);
        if (hd > 96.0f) patrolDir = { hx / hd, hy / hd };
        dir = patrolDir;
        spd = patrolSpeed;
        break;
    }
    }

    Vector2 delta = { dir.x * spd * dt, dir.y * spd * dt };

    // Evita explícitamente que un enemigo cruce físicamente hacia la aldea segura
    if (body.x + delta.x < VILLAGE_LIMIT_X) {
        delta.x = 0.0f;
    }

    map.Move(body, delta);
}
