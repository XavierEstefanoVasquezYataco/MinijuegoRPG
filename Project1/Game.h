#pragma once
#include <deque>
#include <string>
#include <vector>
#include "raylib.h"
#include "GameState.h"
#include "TileMap.h"
#include "Player.h"
#include "NPC.h"
#include "CombatUI.h"
#include "MapView.h"
#include "TitleView.h"
#include "DialogueView.h"

class Game {
public:
    Game();
    ~Game();
    void Run();

private:
    static constexpr int SCREEN_W = 960;
    static constexpr int SCREEN_H = 540;

    GameState state = GameState::TITLE_SCREEN;
    GameState prevState = GameState::MAP;   // para reanudar desde la pausa
    CombatPhase phase = CombatPhase::PLAYER_TURN;
    CombatResult result = CombatResult::NONE;

    TileMap map;
    Player player;
    std::vector<Enemy> enemies;
    Enemy* currentEnemy = nullptr;

    std::vector<NPC> npcs;
    const NPC* nearbyNpc = nullptr;
    const NPC* activeNpc = nullptr;
    int dialogueIndex = 0;
    std::vector<std::string> currentDialogue;
    bool isDialogueConfirmation = false;
    int dialogueChoice = 0; // 0 = Si, 1 = No

    Camera2D camera{};

    CombatUI ui;
    MapView mapView;
    TitleView titleView;
    DialogueView dialogueView;

    std::deque<std::string> combatLog;
    int selectedButton = 0;
    int pauseSel = 0;
    int titleSel = 0;
    int classSel = 0;
    bool confirmExit = false;
    bool continueAvailable = false;
    float timer = 0.0f;
    float mapCooldown = 0.0f;
    bool showMinimap = true;
    bool quit = false;

    void InitWorld();
    void Reset();
    void StartNewGame(HeroClass chosenClass);

    void Update(float dt);
    void Draw();
    void DrawScene(GameState s);

    void UpdateCamera();
    void UpdateTitle();
    void UpdateClassSelect();
    void UpdateControls();
    void UpdateMap(float dt);
    void UpdateDialogue();
    void UpdateCombat(float dt);
    void UpdatePause();

    void Pause();
    void Resume();
    void ActivatePause(int option);

    void StartDialogue(const NPC& npc);
    void EndDialogue();

    void StartCombat(Enemy* e);
    void ExecutePlayerAction(PlayerAction a);
    void EnemyTurn();
    void CheckCombatEnd();
    void FinishCombat();
    void AddLog(const std::string& msg);

    void DrawEndScreen(const char* title, Color c) const;
    void DrawConfirmExit() const;
};
