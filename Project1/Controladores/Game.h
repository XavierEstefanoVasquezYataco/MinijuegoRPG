/**
 * @file Game.h
 * @brief Controlador orquestador principal del videojuego Mini Aventura RPG.
 *
 * Capa de Arquitectura: Controlador (Controller) - Orquestación y Máquina de Estados.
 *
 * Propósito y Responsabilidades:
 * - Ser el mediador central del patrón MVC: captura las entradas físicas de hardware (teclado y ratón de Raylib),
 *   actualiza los modelos de datos (posiciones, vida, maná, progreso de compuertas) y delega
 *   el renderizado a las vistas pasivas.
 * - Administrar la máquina de estados de alto nivel (`state`) y las transiciones (Título, Clases, Controles,
 *   Exploración, Diálogos, Combate, Pausa, Game Over, Victoria).
 * - Controlar la cámara 2D con encuadre suave y restricción de márgenes (*clamping*) dentro del mundo de 1920x1024 px.
 * - Coordinar el combate por turnos: captura de elecciones en la grilla 2x2, cálculo de daño, turnos enemigos
 *   y activación de disparadores visuales en `CombatPresentation`.
 *
 * Relaciones Principales:
 * - Instanciado y ejecutado por el punto de entrada [main.cpp](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/main.cpp).
 * - Posee e inicializa las instancias del Modelo: [Modelo/TileMap.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/TileMap.h),
 *   [Modelo/Player.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/Player.h), [Modelo/NPC.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Modelo/NPC.h).
 * - Posee e invoca a las Vistas pasivas: [Vistas/MapView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/MapView.h),
 *   [Vistas/CombatUI.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatUI.h), [Vistas/TitleView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/TitleView.h),
 *   [Vistas/DialogueView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/DialogueView.h) y el módulo [Vistas/CombatPresentation.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatPresentation.h).
 */

#pragma once
#include <deque>
#include <string>
#include <vector>
#include "raylib.h"
#include "../Modelo/GameState.h"
#include "../Modelo/TileMap.h"
#include "../Modelo/Player.h"
#include "../Modelo/NPC.h"
#include "../Vistas/CombatUI.h"
#include "../Vistas/MapView.h"
#include "../Vistas/TitleView.h"
#include "../Vistas/DialogueView.h"
#include "../Vistas/CombatPresentation.h"
#include "AudioManager.h"
#include "SaveSystem.h"

/**
 * @class Game
 * @brief Orquestador central de lógica, eventos y renderizado de la aplicación.
 */
class Game {
public:
    /**
     * @brief Constructor. Inicializa la ventana de Raylib (960x540 px), 60 FPS fijos,
     *        carga texturas y prepara el estado inicial (`TITLE_SCREEN`).
     */
    Game();

    /**
     * @brief Destructor. Descarga texturas de memoria y cierra la ventana de Raylib.
     */
    ~Game();

    /**
     * @brief Bucle principal de la aplicación.
     *
     * Itera mientras `WindowShouldClose()` sea false y la bandera `quit` no esté activa.
     * En cada iteración calcula el tiempo transcurrido (`GetFrameTime()`), actualiza la lógica
     * mediante `Update(dt)` y dibuja la escena mediante `Draw()`.
     */
    void Run();

private:
    static constexpr int SCREEN_W = 960;    ///< Ancho de la resolución virtual del juego.
    static constexpr int SCREEN_H = 540;    ///< Alto de la resolución virtual del juego.

    // Máquina de estados
    GameState state = GameState::TITLE_SCREEN;  ///< Estado global actual.
    GameState prevState = GameState::MAP;       ///< Estado previo para reanudar desde la pausa.
    CombatPhase phase = CombatPhase::PLAYER_TURN; ///< Subfase del combate por turnos.
    CombatResult result = CombatResult::NONE;   ///< Resultado del último combate.

    // Entidades del Modelo
    TileMap map;                                ///< Matriz del mundo de 60x32 tiles.
    Player player;                              ///< Instancia del héroe con atributos y posición.
    std::vector<Enemy> enemies;                 ///< Colección de monstruos en el supramundo.
    Enemy* currentEnemy = nullptr;              ///< Puntero al enemigo contra el que se combate.

    // NPCs y Diálogos
    std::vector<NPC> npcs;                      ///< Habitantes de la aldea y carteles.
    const NPC* nearbyNpc = nullptr;             ///< NPC más cercano en rango de interacción.
    const NPC* activeNpc = nullptr;             ///< NPC con el que se está conversando activamente.
    int dialogueIndex = 0;                      ///< Índice del párrafo activo en la conversación.
    std::vector<std::string> currentDialogue;   ///< Frases del diálogo en curso.
    bool isDialogueConfirmation = false;        ///< true si requiere confirmación interactiva Sí/No.
    int dialogueChoice = 0;                     ///< 0 para Sí, 1 para No.

    // Cámara 2D
    Camera2D camera{};                          ///< Cámara de seguimiento suave del jugador.

    // Vistas pasivas
    CombatUI ui;                                ///< Vista de la arena de combate.
    MapView mapView;                            ///< Vista del supramundo, HUD y minimapa.
    TitleView titleView;                        ///< Vista de menús y selección de héroes.
    DialogueView dialogueView;                  ///< Vista de cuadros de diálogo.
    CombatPresentation combatPres;              ///< Módulo de efectos y animaciones de combate.

    // Variables de control de interfaz y temporizadores
    std::deque<std::string> combatLog;          ///< Registro de los últimos mensajes de combate.
    int selectedButton = 0;                     ///< Botón seleccionado en el menú 2x2 de combate.
    int pauseSel = 0;                           ///< Opción seleccionada en el menú de pausa.
    int titleSel = 0;                           ///< Opción seleccionada en la pantalla de título.
    int classSel = 0;                           ///< Héroe seleccionado en la pantalla de clases.
    bool confirmExit = false;                   ///< Bandera que activa el cuadro de confirmación al salir.
    bool continueAvailable = false;             ///< true si existe una partida guardada válida que se pueda continuar.
    float timer = 0.0f;                         ///< Temporizador para retardos de turnos en combate.
    float mapCooldown = 0.0f;                   ///< Tiempo de gracia tras escapar para evitar re-iniciar combates inmediatos.
    bool showMinimap = true;                    ///< Alternable con la tecla M.
    bool quit = false;                          ///< Si es true, finaliza el ciclo `Run()`.
    bool combatRewardClaimed = false;           ///< Garantiza entregar EXP y subidas de nivel una sola vez por combate.
    GameState lastState = GameState::MAP;
    float saveMsgTimer = 0.0f;
    const char* saveMsg = "";
    AudioManager audio;

    // Métodos de inicialización y reinicio
    void InitWorld();
    void Reset();
    void StartNewGame(HeroClass chosenClass);

    // Bucle y máquina de estados
    void Update(float dt);
    void Draw();
    void DrawScene(GameState s);

    // Actualizaciones especializadas por escena
    void UpdateCamera();
    void UpdateTitle();
    void UpdateClassSelect();
    void UpdateControls();
    void UpdateMap(float dt);
    void UpdateDialogue();
    void UpdateCombat(float dt);
    void UpdatePause();

    // Gestión de Pausa
    void Pause();
    void Resume();
    void ActivatePause(int option);
    bool SaveGame() const;
    bool LoadGame();

    // Gestión de Diálogos
    void StartDialogue(const NPC& npc);
    void EndDialogue();

    // Gestión de Combate
    void StartCombat(Enemy* e);
    void ExecutePlayerAction(PlayerAction a);
    void EnemyTurn();
    void CheckCombatEnd();
    void FinishCombat();
    void AddLog(const std::string& msg);

    // Pantallas finales
    void DrawEndScreen(const char* title, Color c) const;
    void DrawConfirmExit() const;
};
