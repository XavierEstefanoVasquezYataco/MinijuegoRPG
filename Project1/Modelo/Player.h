/**
 * @file Player.h
 * @brief Modelos de entidades de combate: Combatant, Player, Enemy y tablas de configuración.
 *
 * Capa de Arquitectura: Modelo (Model) - Dominio de Combate y Progresión.
 *
 * Propósito y Responsabilidades:
 * - Centralizar los datos y reglas de combate por turnos (fórmulas de daño, puntos de vida y maná).
 * - Definir la clase base polimórfica `Combatant` para compartir lógica de estadísticas entre héroe y enemigos.
 * - Modelar la clase `Player` (héroe): posición en mapa, arquetipos, velocidad, inventario de pociones,
 *   acumulación de experiencia y subidas de nivel escalonadas.
 * - Modelar la clase `Enemy` (monstruo): inteligencia artificial de mapa (patrulla y persecución),
 *   recompensas de EXP y desbloqueo de compuertas.
 * - Proveer tablas de solo lectura para arquetipos de héroe (`HeroClassData`) y especies de monstruos (`MonsterSpecies`).
 *
 * Relaciones Principales:
 * - Utilizado por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) para actualizar la lógica de exploración y combate.
 * - Utilizado por las Vistas ([Vistas/MapView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/MapView.h), [Vistas/CombatUI.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatUI.h), [Vistas/TitleView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/TitleView.h)) para consultar atributos mediante referencias constantes (`const &`).
 * - Se apoya en [Modelo/TileMap.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/TileMap.h) para la resolución de colisiones y límites del mundo.
 *
 * Observaciones Arquitectónicas:
 * - Representa el núcleo del modelo de datos. Aunque incluye tipos de Raylib (`Rectangle`, `Vector2`, `Color`),
 *   no realiza ninguna operación de renderizado ni lee teclado/ratón de manera directa.
 */

#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "GameState.h"
#include "TileMap.h"

/**
 * @struct HeroClassData
 * @brief Registro inmutable con los atributos base, descripciones y habilidades de cada arquetipo de héroe.
 */
struct HeroClassData {
    HeroClass type;             ///< Arquetipo correspondiente (WARRIOR, MAGE, SCOUT).
    const char* name;           ///< Nombre legible en menús e interfaz ("Guerrero", "Maga", "Exploradora").
    int hp, mp, atk, def;       ///< Estadísticas iniciales de nivel 1.
    float speed;                ///< Velocidad de desplazamiento en el mapa (px/s).
    const char* skillName;      ///< Nombre de la técnica especial.
    int skillCost;              ///< Coste en puntos de maná (MP) para ejecutar la técnica.
    float skillMultiplier;      ///< Multiplicador sobre el atributo ATK al calcular daño de habilidad.
    int fleeChance;             ///< Porcentaje de éxito al intentar huir del combate (ej. 50%, 65%).
    const char* description;    ///< Resumen pedagógico del rol para la pantalla de selección.
    const char* skillEffect;    ///< Breve descripción del impacto táctico de su técnica.
    int artId;                  ///< Identificador de sprite procedimental para la tarjeta de clase.
    Color bodyColor;            ///< Color primario del ropaje/armadura.
    Color lightColor;           ///< Color de ribetes decorativos.
};

/**
 * @brief Consulta la tabla de configuración estática para un arquetipo de héroe dado.
 * @param type Clase solicitada (WARRIOR, MAGE, SCOUT).
 * @return const HeroClassData& Referencia inmutable a los datos del arquetipo.
 */
const HeroClassData& GetHeroClassData(HeroClass type);

/**
 * @struct MonsterSpecies
 * @brief Datos inmutables que definen una especie de monstruo en el juego.
 */
struct MonsterSpecies {
    char marker;                ///< Letra que representa su posición en la plantilla de TileMap ('A'..'F').
    const char* name;           ///< Nombre que se muestra en combate ("Slime", "Murcielago", "Dragon", etc.).
    int level;                  ///< Nivel representativo de la criatura.
    int hp, atk, def;           ///< Estadísticas de combate base.
    bool boss;                  ///< true si es el jefe final (el Dragón en las Ruinas).
    int gate;                   ///< Índice de compuerta que desbloquea al morir (0 = compuerta 1, 1 = compuerta 2, -1 = ninguna).
    int art;                    ///< Índice de arte procedimental (blob, murciélago, humanoide, bestia, dragón).
    Color body, light;          ///< Colores asignados a su dibujo.
    int expReward;              ///< Puntos de experiencia otorgados al jugador al ser vencido.
};

/**
 * @brief Obtiene la tabla completa de especies de criaturas registradas en el juego.
 * @return const std::vector<MonsterSpecies>& Vector estático con las 6 especies de monstruos.
 */
const std::vector<MonsterSpecies>& GetSpecies();

/**
 * @brief Busca una especie de monstruo a partir del carácter de la celda del mapa.
 * @param marker Letra entre 'A' y 'F'.
 * @return const MonsterSpecies* Puntero a la especie encontrada o nullptr si no existe.
 */
const MonsterSpecies* FindSpecies(char marker);

/**
 * @class Combatant
 * @brief Clase base abstracta/polimórfica para cualquier entidad que participe en el combate por turnos.
 *
 * Gestiona estadísticas numéricas fundamentales (salud, maná, ataque y defensa),
 * recepción de daño con mitigación y cálculo estandarizado de daño con varianza.
 */
class Combatant {
public:
    std::string name;           ///< Nombre visible en combate.
    int level = 1;              ///< Nivel actual de la entidad.
    int hp = 1, maxHp = 1;      ///< Puntos de vida actuales y máximos.
    int mp = 0, maxMp = 0;      ///< Puntos de maná actuales y máximos.
    int atk = 1, def = 0;       ///< Poder de ataque físico y defensa reductora de daño.

    Combatant() = default;

    /**
     * @brief Constructor con parámetros para inicializar estadísticas numéricas.
     * @param n Nombre del combatiente.
     * @param hp_ Puntos de salud máximos e iniciales.
     * @param mp_ Puntos de maná máximos e iniciales.
     * @param atk_ Poder de ataque.
     * @param def_ Armadura o mitigación de defensa.
     */
    Combatant(const std::string& n, int hp_, int mp_, int atk_, int def_);

    virtual ~Combatant() = default;

    /**
     * @brief Comprueba si el combatiente sigue con vida (HP > 0).
     * @return true Si su salud actual es mayor que cero.
     */
    bool IsAlive() const { return hp > 0; }

    /**
     * @brief Reduce los puntos de salud de la entidad asegurando no bajar de 0.
     * @param amount Cantidad bruta de daño a restar.
     */
    void TakeDamage(int amount);

    /**
     * @brief Aumenta la salud actual respetando estrictamente el límite superior `maxHp`.
     * @param amount Puntos de vida a recuperar.
     */
    void Heal(int amount);

    /**
     * @brief Calcula el daño saliente contra un combatiente objetivo con varianza pseudoaleatoria.
     *
     * Fórmula: daño = max(1, (atk * multiplier) - target.def + varianza[-2, +3]).
     *
     * @param target Combatiente defensor cuya defensa restará valor al impacto.
     * @param multiplier Factor multiplicador sobre ATK (ej. 1.0f para ataque normal, >1.0f para habilidades).
     * @return int Daño neto resultante (mínimo garantizado de 1).
     */
    int CalculateDamage(const Combatant& target, float multiplier = 1.0f) const;
};

/**
 * @struct StatGains
 * @brief Incrementos numéricos obtenidos al subir de nivel.
 */
struct StatGains {
    int maxHp = 0;  ///< Aumento sobre la salud máxima.
    int maxMp = 0;  ///< Aumento sobre el maná máximo.
    int atk = 0;    ///< Aumento sobre el poder de ataque.
    int def = 0;    ///< Aumento sobre la armadura de defensa.
};

/**
 * @struct ExpResult
 * @brief Resume los resultados acumulados tras otorgar puntos de experiencia (EXP) al héroe.
 */
struct ExpResult {
    bool leveledUp = false;     ///< true si el héroe subió al menos un nivel.
    int levelsGained = 0;       ///< Cantidad de niveles alcanzados en esta asignación de EXP.
    StatGains totalGains;       ///< Suma de todas las mejoras de atributos obtenidas.
};

/**
 * @class Player
 * @brief Entidad controlada por el usuario; especialización de `Combatant`.
 *
 * Incorpora mecánicas de exploración (posición, velocidad, animaciones, orientación),
 * inventario de pociones curativas, progresión de nivel y consumo de maná para habilidades.
 */
class Player : public Combatant {
public:
    HeroClass heroClass = HeroClass::WARRIOR;    ///< Arquetipo activo.
    Rectangle body{ 0, 0, 24, 24 };              ///< Rectángulo de colisión y renderizado en el supramundo.
    float speed = 180.0f;                       ///< Rapidez de movimiento en píxeles por segundo.
    int potions = 3;                            ///< Existencias de pociones en el inventario.

    int exp = 0;                                ///< Experiencia acumulada en el nivel actual.
    int expNext = 50;                           ///< Experiencia requerida para subir al siguiente nivel.

    Facing facing = Facing::DOWN;               ///< Orientación actual de la mirada.
    bool moving = false;                        ///< Indica si se está desplazando en el fotograma actual.
    float animTimer = 0.0f;                     ///< Acumulador de tiempo para la animación de pasos.

    /// Cantidad fija de salud restaurada por cada poción (40 HP).
    static constexpr int POTION_HEAL = 40;

    /**
     * @brief Constructor del jugador. Ubica al héroe en la aldea segura (col 4, row 14).
     */
    Player();

    /**
     * @brief Configura las estadísticas y arquetipo inicial del jugador según la clase elegida.
     * @param cls Arquetipo seleccionado en el menú (Guerrero, Maga, Exploradora).
     */
    void InitClass(HeroClass cls);

    const char* GetClassName() const;
    const char* GetSkillName() const;
    int GetSkillCost() const;
    float GetSkillMultiplier() const;
    int GetFleeChance() const;

    /**
     * @brief Calcula el vector de desplazamiento deseado según la entrada y el tiempo delta.
     * @param inputDir Vector direccional normalizado (-1..1 en x e y).
     * @param dt Tiempo transcurrido en el fotograma en segundos.
     * @return Vector2 Desplazamiento escalado por la velocidad del personaje.
     */
    Vector2 GetDesiredMove(Vector2 inputDir, float dt) const;

    /**
     * @brief Actualiza la orientación, temporizador de animación y posición contra el mapa.
     * @param inputDir Dirección de movimiento ingresada por el usuario.
     * @param dt Tiempo transcurrido en segundos.
     * @param map Referencia al mapa para aplicar colisiones físicas.
     */
    void Update(Vector2 inputDir, float dt, const TileMap& map);

    /**
     * @brief Devuelve las coordenadas centrales del héroe en el supramundo.
     * @return Vector2 Coordenadas (x + width/2, y + height/2).
     */
    Vector2 Center() const { return { body.x + body.width / 2.0f, body.y + body.height / 2.0f }; }

    /**
     * @brief Comprueba si el héroe tiene suficiente maná para ejecutar su habilidad.
     */
    bool CanUseSkill() const { return mp >= GetSkillCost(); }

    /**
     * @brief Descuenta el coste en MP de la habilidad si se cuenta con la cantidad requerida.
     * @return true Si se pudo descontar el maná; false si los puntos eran insuficientes.
     */
    bool UseSkillCost();

    /**
     * @brief Comprueba si es posible consumir una poción (inventario > 0 y HP < maxHp).
     */
    bool CanUsePotion() const { return potions > 0 && hp < maxHp; }

    /**
     * @brief Consume una poción del inventario y cura al jugador hasta un máximo de 40 HP.
     * @return int Cantidad de puntos de vida efectivamente restaurados (0 si no había o ya estaba lleno).
     */
    int UsePotion();

    /**
     * @brief Añade experiencia, calculando subidas continuas de nivel si se supera el umbral.
     *
     * Regla pedagógica: La subida de nivel incrementa los topes máximos de estadísticas,
     * pero NO cura automáticamente la salud ni el maná actuales (persistencia estricta).
     *
     * @param amount Puntos de EXP a acreditar.
     * @return ExpResult Estructura con el informe de niveles ganados y aumentos de estadísticas.
     */
    ExpResult GainExp(int amount);

    /**
     * @brief Ejecuta el incremento de un nivel aplicando las curvas de crecimiento de su clase.
     * @return StatGains Atributos añadidos a maxHp, maxMp, atk y def.
     */
    StatGains LevelUp();

    /**
     * @brief Restaura completamente los puntos de salud y maná actuales a sus valores máximos.
     *
     * Invocada por la Curandera Elena tras la confirmación afirmativa del jugador.
     */
    void FullRestore();
};

/**
 * @class Enemy
 * @brief Criatura hostil presente en el supramundo y oponente en combates; especialización de `Combatant`.
 *
 * Gestiona su comportamiento de patrulla alrededor de un punto de origen (`home`) y
 * persecución inteligente del jugador cuando entra en su radio de visión,
 * respetando la frontera inviolable de la aldea segura.
 */
class Enemy : public Combatant {
public:
    Rectangle body{ 0, 0, 24, 24 };      ///< Caja delimitadora en el mapa (24x24 px).
    bool defeated = false;              ///< true si el enemigo ha sido derrotado y debe desaparecer del mapa.
    bool isBoss = false;                ///< true si es el Dragón, provocando la victoria al morir.
    int unlocksWall = -1;               ///< Índice de compuerta a abrir al morir (-1 si ninguna).
    int expReward = 0;                  ///< Experiencia otorgada al ser vencido.

    int art = 0;                        ///< Identificador del sprite procedimental (PixelArt).
    Color bodyColor{ 255, 255, 255, 255 };
    Color lightColor{ 255, 255, 255, 255 };

    AIState ai = AIState::PATROL;       ///< Estado de la IA (PATROL o CHASE).
    Vector2 home{ 0, 0 };               ///< Coordenadas originales donde nació el enemigo.
    Vector2 patrolDir{ 0, 0 };          ///< Dirección aleatoria en la que patrulla actualmente.
    float patrolTimer = 0.0f;           ///< Temporizador restante antes de cambiar de dirección de patrulla.
    float patrolSpeed = 40.0f;          ///< Rapidez de movimiento al patrullar (px/s).
    float chaseSpeed = 75.0f;           ///< Rapidez acelerada al perseguir al jugador (px/s).
    float detectRadius = 130.0f;        ///< Distancia a la que detecta al jugador y entra en persecución.
    float loseRadius = 220.0f;          ///< Distancia a la que pierde de vista al jugador y vuelve a patrullar.

    /**
     * @brief Construye un enemigo a partir de una plantilla de especie y coordenadas de casilla.
     * @param sp Plantilla inmutable con estadísticas de la criatura.
     * @param col Columna del mapa donde se encontró el marcador.
     * @param row Fila del mapa donde se encontró el marcador.
     */
    Enemy(const MonsterSpecies& sp, int col, int row);

    /**
     * @brief Devuelve las coordenadas centrales del enemigo en píxeles.
     */
    Vector2 Center() const { return { body.x + body.width / 2.0f, body.y + body.height / 2.0f }; }

    /**
     * @brief Actualiza la máquina de estados de la IA y mueve al enemigo en el mapa.
     *
     * Reglas implementadas:
     * 1. Si el jugador está dentro de la Aldea Segura (x < 17 * TILE), el enemigo no lo persigue.
     * 2. Si el enemigo detecta al jugador a menos de `detectRadius`, pasa a CHASE.
     * 3. Si se aleja a más de `loseRadius`, vuelve a PATROL y retorna hacia `home`.
     * 4. Los enemigos tienen prohibido cruzar físicamente la frontera de la aldea.
     *
     * @param dt Tiempo transcurrido en el fotograma.
     * @param target Coordenadas centrales del jugador a seguir.
     * @param map Mapa de colisiones para evitar atravesar árboles o muros.
     */
    void UpdateAI(float dt, Vector2 target, const TileMap& map);
};
