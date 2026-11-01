#include "Game.h"
#include <algorithm>
#include <cmath>
#include <string>
#include "raylib.h"

// ------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR / BUCLE
// ------------------------------------------------------------

Game::Game() {
    InitWindow(SCREEN_W, SCREEN_H, "Mini Aventura RPG");
    SetTargetFPS(60);

    // Desactivar el cierre automatico de Raylib por tecla ESCAPE
    // para centralizar el comportamiento de Escape en el controlador
    SetExitKey(KEY_NULL);

    mapView.Load();
    InitWorld();
    state = GameState::TITLE_SCREEN;
}

Game::~Game() {
    mapView.Unload();
    CloseWindow();
}

void Game::InitWorld() {
    map = TileMap();
    player = Player();
    enemies.clear();

    const std::vector<TileMap::Spawn> spawns = map.ExtractSpawns();
    for (const auto& sp : spawns) {
        const MonsterSpecies* species = FindSpecies(sp.id);
        if (species != nullptr) {
            enemies.emplace_back(*species, sp.col, sp.row);
        }
    }

    npcs = CreateWorldNPCs();
    nearbyNpc = nullptr;
    activeNpc = nullptr;
    dialogueIndex = 0;
    currentDialogue.clear();
    isDialogueConfirmation = false;
    dialogueChoice = 0;

    currentEnemy = nullptr;
    mapCooldown = 0.0f;
    showMinimap = true;
    confirmExit = false;

    // Configuracion de la camara 2D centrada en el jugador
    camera.offset = { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    UpdateCamera();
}

void Game::Reset() {
    InitWorld();
    combatPres.Reset();
}

void Game::UpdateCamera() {
    const Vector2 target = player.Center();
    const float halfW = SCREEN_W / 2.0f;
    const float halfH = SCREEN_H / 2.0f;
    const float mapPixelW = static_cast<float>(TileMap::COLS * TileMap::TILE);
    const float mapPixelH = static_cast<float>(TileMap::ROWS * TileMap::TILE);

    camera.target.x = std::clamp(target.x, halfW, mapPixelW - halfW);
    camera.target.y = std::clamp(target.y, halfH, mapPixelH - halfH);
}

void Game::StartNewGame(HeroClass chosenClass) {
    Reset();
    player.InitClass(chosenClass);
    UpdateCamera();
    state = GameState::MAP;
}

void Game::Run() {
    while (!WindowShouldClose() && !quit) {
        const float dt = GetFrameTime();
        Update(dt);
        Draw();
    }
}

// ------------------------------------------------------------
// PAUSA
// ------------------------------------------------------------

void Game::Pause() {
    prevState = state;
    state = GameState::PAUSED;
    pauseSel = 0;
    confirmExit = false;
}

void Game::Resume() {
    state = prevState;
    confirmExit = false;
}

void Game::ActivatePause(int option) {
    switch (option) {
    case 0: // Reanudar
        Resume();
        break;
    case 1: { // Reiniciar (conserva el heroe elegido)
        const HeroClass currentClass = player.heroClass;
        Reset();
        player.InitClass(currentClass);
        Resume();
        break;
    }
    case 2: // Volver al titulo (solicita confirmacion)
        confirmExit = true;
        break;
    default:
        break;
    }
}

void Game::UpdatePause() {
    if (confirmExit) {
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_ENTER)) {
            state = GameState::TITLE_SCREEN;
            confirmExit = false;
            Reset();
        }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
            confirmExit = false;
        }
        return;
    }

    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        Resume();
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        pauseSel = (pauseSel + 1) % 3;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        pauseSel = (pauseSel + 2) % 3;
    }

    const int hovered = mapView.GetPauseButton(GetMousePosition());

    if (hovered >= 0) {
        pauseSel = hovered;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ActivatePause(pauseSel);
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered >= 0) {
        ActivatePause(hovered);
    }
}

// ------------------------------------------------------------
// PANTALLAS DE TITULO, CLASES Y CONTROLES
// ------------------------------------------------------------

void Game::UpdateTitle() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        titleSel = (titleSel + 1) % 4;
        if (titleSel == 1 && !continueAvailable) {
            titleSel = 2; // Saltar opcion desactivada
        }
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        titleSel = (titleSel + 3) % 4;
        if (titleSel == 1 && !continueAvailable) {
            titleSel = 0; // Saltar opcion desactivada
        }
    }

    const Vector2 mouse = GetMousePosition();
    const int hovered = titleView.GetTitleButton(mouse);
    if (hovered >= 0) {
        if (hovered != 1 || continueAvailable) {
            titleSel = hovered;
        }
    }

    auto activateOption = [this](int opt) {
        switch (opt) {
        case 0: // Nueva partida
            classSel = 0;
            state = GameState::CLASS_SELECT;
            break;
        case 1: // Continuar (desactivado en esta etapa)
            if (continueAvailable) {
                // Se integrara en la Etapa 7
            }
            break;
        case 2: // Controles
            state = GameState::CONTROLS;
            break;
        case 3: // Salir
            quit = true;
            break;
        }
    };

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        activateOption(titleSel);
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered >= 0) {
        if (hovered != 1 || continueAvailable) {
            activateOption(hovered);
        }
    }
}

void Game::UpdateClassSelect() {
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        classSel = (classSel + 1) % 3;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        classSel = (classSel + 2) % 3;
    }

    const Vector2 mouse = GetMousePosition();
    const int hoveredCard = titleView.GetClassCard(mouse);
    if (hoveredCard >= 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        classSel = hoveredCard;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::TITLE_SCREEN;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        StartNewGame(static_cast<HeroClass>(classSel));
        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (titleView.IsOverBackButton(mouse, GameState::CLASS_SELECT)) {
            state = GameState::TITLE_SCREEN;
            return;
        }
        if (titleView.IsOverStartButton(mouse)) {
            StartNewGame(static_cast<HeroClass>(classSel));
            return;
        }
    }
}

void Game::UpdateControls() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        state = GameState::TITLE_SCREEN;
        return;
    }

    const Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (titleView.IsOverBackButton(mouse, GameState::CONTROLS)) {
            state = GameState::TITLE_SCREEN;
            return;
        }
    }
}

// ------------------------------------------------------------
// ACTUALIZACION GENERAL
// ------------------------------------------------------------

void Game::Update(float dt) {
    switch (state) {
    case GameState::TITLE_SCREEN:
        UpdateTitle();
        break;

    case GameState::CLASS_SELECT:
        UpdateClassSelect();
        break;

    case GameState::CONTROLS:
        UpdateControls();
        break;

    case GameState::MAP:
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
            Pause();
            break;
        }
        UpdateMap(dt);
        break;

    case GameState::DIALOGUE:
        UpdateDialogue();
        break;

    case GameState::COMBAT:
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
            Pause();
            break;
        }
        UpdateCombat(dt);
        break;

    case GameState::PAUSED:
        UpdatePause();
        break;

    case GameState::GAME_OVER:
    case GameState::VICTORY:
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            // Reinicia la partida conservando el mismo heroe
            const HeroClass currentClass = player.heroClass;
            Reset();
            player.InitClass(currentClass);
            UpdateCamera();
            state = GameState::MAP;
        }
        else if (IsKeyPressed(KEY_ESCAPE)) {
            // Vuelve a la pantalla de titulo
            state = GameState::TITLE_SCREEN;
            Reset();
        }
        break;
    }
}

// ------------------------------------------------------------
// MAPA
// ------------------------------------------------------------

void Game::UpdateMap(float dt) {
    if (IsKeyPressed(KEY_M)) {
        showMinimap = !showMinimap;
    }

    if (mapCooldown > 0.0f) {
        mapCooldown -= dt;
    }

    // Lectura de entrada del jugador en el controlador (MVC)
    Vector2 inputDir{ 0.0f, 0.0f };
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    inputDir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  inputDir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  inputDir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) inputDir.x += 1.0f;

    // Desplazamiento y orientacion del jugador
    Vector2 m = player.GetDesiredMove(inputDir, dt);
    player.moving = (m.x != 0.0f || m.y != 0.0f);
    if (player.moving) {
        if (std::fabs(m.x) > std::fabs(m.y)) player.facing = (m.x < 0) ? Facing::LEFT : Facing::RIGHT;
        else                                 player.facing = (m.y < 0) ? Facing::UP : Facing::DOWN;
        player.animTimer += dt;
    } else {
        player.animTimer = 0.0f;
    }

    // Movimiento en X con resolucion de colisiones contra tiles y personajes fisicos
    Rectangle testX = player.body;
    testX.x += m.x;
    bool blockedX = map.IsBlocked(testX);
    if (!blockedX) {
        for (const auto& npc : npcs) {
            if (npc.isSolid && CheckCollisionRecs(testX, npc.body)) {
                blockedX = true;
                break;
            }
        }
    }
    if (!blockedX) player.body.x = testX.x;

    // Movimiento en Y con resolucion de colisiones contra tiles y personajes fisicos
    Rectangle testY = player.body;
    testY.y += m.y;
    bool blockedY = map.IsBlocked(testY);
    if (!blockedY) {
        for (const auto& npc : npcs) {
            if (npc.isSolid && CheckCollisionRecs(testY, npc.body)) {
                blockedY = true;
                break;
            }
        }
    }
    if (!blockedY) player.body.y = testY.y;

    // Actualiza la posicion de la camara de seguimiento
    UpdateCamera();

    // Deteccion determinista del elemento interactuable mas cercano (radio 46 px)
    nearbyNpc = nullptr;
    float minDistance = 46.0f;
    const Vector2 pc = player.Center();
    for (const auto& npc : npcs) {
        const Vector2 nc = npc.Center();
        const float dx = pc.x - nc.x;
        const float dy = pc.y - nc.y;
        const float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < minDistance) {
            minDistance = dist;
            nearbyNpc = &npc;
        }
    }

    if (nearbyNpc != nullptr && IsKeyPressed(KEY_E)) {
        StartDialogue(*nearbyNpc);
        return;
    }

    // Actualiza la IA simple de los enemigos.
    for (Enemy& enemy : enemies) {
        enemy.UpdateAI(dt, player.Center(), map);
    }

    // Tocar a un enemigo inicia el combate.
    if (mapCooldown <= 0.0f) {
        for (Enemy& enemy : enemies) {
            if (!enemy.defeated &&
                CheckCollisionRecs(player.body, enemy.body)) {
                StartCombat(&enemy);
                break;
            }
        }
    }
}

// ------------------------------------------------------------
// DIALOGOS E INTERACCION CON NPCS
// ------------------------------------------------------------

void Game::StartDialogue(const NPC& npc) {
    activeNpc = &npc;

    // Evaluacion de progreso consultando la fuente de verdad existente
    const bool gate1Open = (map.At(28, 14) == 'p');
    const bool gate2Open = (map.At(39, 14) == 'p');
    bool bossDefeated = false;
    for (const auto& e : enemies) {
        if (e.isBoss && e.defeated) {
            bossDefeated = true;
            break;
        }
    }

    currentDialogue = npc.GetDialogue(gate1Open, gate2Open, bossDefeated);
    dialogueIndex = 0;
    isDialogueConfirmation = (npc.type == NPCType::HEALER);
    dialogueChoice = 0; // Por defecto: Si
    state = GameState::DIALOGUE;
}

void Game::UpdateDialogue() {
    if (isDialogueConfirmation) {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            dialogueChoice = 0;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            dialogueChoice = 1;
        }

        const Vector2 mouse = GetMousePosition();
        const int hovered = dialogueView.GetHoveredChoice(mouse);
        if (hovered >= 0) {
            dialogueChoice = hovered;
        }

        auto applyHeal = [this]() {
            player.FullRestore();
            bool repuso = false;
            if (player.potions < 2) {
                player.potions = 2;
                repuso = true;
            }
            isDialogueConfirmation = false;
            currentDialogue = {
                repuso
                    ? "Tu salud y mana han sido restaurados al maximo. Tus pociones se han completado a 2."
                    : "Tu salud y mana han sido restaurados al maximo. Ya contabas con suficientes pociones."
            };
            dialogueIndex = 0;
        };

        auto cancelHeal = [this]() {
            isDialogueConfirmation = false;
            currentDialogue = {
                "Entendido. Vuelve cuando necesites descansar y recuperar tus fuerzas."
            };
            dialogueIndex = 0;
        };

        if (IsKeyPressed(KEY_S)) {
            applyHeal();
            return;
        }
        if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
            cancelHeal();
            return;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || (hovered >= 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
            if (dialogueChoice == 0) applyHeal();
            else cancelHeal();
            return;
        }
    }
    else {
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            dialogueIndex++;
            if (dialogueIndex >= static_cast<int>(currentDialogue.size())) {
                EndDialogue();
            }
        }
        else if (IsKeyPressed(KEY_ESCAPE)) {
            EndDialogue();
        }
    }
}

void Game::EndDialogue() {
    activeNpc = nullptr;
    dialogueIndex = 0;
    currentDialogue.clear();
    isDialogueConfirmation = false;
    state = GameState::MAP;
}

// ------------------------------------------------------------
// COMBATE
// ------------------------------------------------------------

void Game::StartCombat(Enemy* enemy) {
    if (enemy == nullptr) {
        return;
    }

    currentEnemy = enemy;
    state = GameState::COMBAT;
    phase = CombatPhase::PLAYER_TURN;
    result = CombatResult::NONE;
    selectedButton = 0;
    timer = 0.0f;
    combatRewardClaimed = false;

    const int mapCol = enemy != nullptr ? static_cast<int>(enemy->body.x / TileMap::TILE) : 20;
    combatPres.OnCombatStart(player, *enemy, mapCol);

    combatLog.clear();
    AddLog("Aparece un " + enemy->name + "!");
    AddLog("Que hara " + player.name + "?");
}

void Game::AddLog(const std::string& msg) {
    combatLog.push_back(msg);

    while (combatLog.size() > 10) {
        combatLog.pop_front();
    }
}

void Game::UpdateCombat(float dt) {
    if (currentEnemy != nullptr) {
        combatPres.Update(dt, player, *currentEnemy);
    }

    switch (phase) {
    case CombatPhase::PLAYER_TURN: {
        // Menu 2 x 2:
        // 0 Atacar, 1 Habilidad, 2 Pocion, 3 Huir.
        if (IsKeyPressed(KEY_RIGHT) ||
            IsKeyPressed(KEY_LEFT) ||
            IsKeyPressed(KEY_D) ||
            IsKeyPressed(KEY_A)) {
            selectedButton ^= 1;
        }

        if (IsKeyPressed(KEY_UP) ||
            IsKeyPressed(KEY_DOWN) ||
            IsKeyPressed(KEY_W) ||
            IsKeyPressed(KEY_S)) {
            selectedButton ^= 2;
        }

        PlayerAction action = PlayerAction::NONE;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            action = static_cast<PlayerAction>(selectedButton);
        }

        const Vector2 mousePos = GetMousePosition();
        const int hovered = ui.GetHoveredButton(mousePos);

        if (hovered >= 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedButton = hovered;
            action = static_cast<PlayerAction>(hovered);
        }

        if (action != PlayerAction::NONE) {
            ExecutePlayerAction(action);
        }

        break;
    }

    case CombatPhase::ENEMY_WAIT:
        timer -= dt;
        if (timer <= 0.0f) {
            EnemyTurn();
        }
        break;

    case CombatPhase::END_WAIT:
        timer -= dt;
        if (timer <= 0.0f) {
            FinishCombat();
        }
        break;
    }
}

void Game::ExecutePlayerAction(PlayerAction action) {
    if (currentEnemy == nullptr) {
        return;
    }

    switch (action) {
    case PlayerAction::ATTACK: {
        const int dmg = player.CalculateDamage(*currentEnemy);
        currentEnemy->TakeDamage(dmg);
        combatPres.TriggerPlayerAttack(dmg);
        AddLog(player.name + " ataca causando " + std::to_string(dmg) + " pts.");

        if (!currentEnemy->IsAlive()) {
            CheckCombatEnd();
        }
        else {
            phase = CombatPhase::ENEMY_WAIT;
            timer = 1.0f;
        }
        break;
    }

    case PlayerAction::SKILL: {
        if (!player.CanUseSkill()) {
            AddLog("No tienes suficiente mana! (" + std::to_string(player.GetSkillCost()) + " MP)");
            return;
        }

        player.UseSkillCost();
        const int dmg = player.CalculateDamage(
            *currentEnemy,
            player.GetSkillMultiplier()
        );

        currentEnemy->TakeDamage(dmg);
        combatPres.TriggerPlayerSkill(player.heroClass, dmg);
        AddLog(player.name + " usa " + player.GetSkillName() + "!");
        AddLog("Impacto critico por " + std::to_string(dmg) + " pts!");

        if (!currentEnemy->IsAlive()) {
            CheckCombatEnd();
        }
        else {
            phase = CombatPhase::ENEMY_WAIT;
            timer = 1.0f;
        }
        break;
    }

    case PlayerAction::POTION: {
        if (player.potions <= 0) {
            AddLog("No te quedan pociones!");
            return;
        }

        if (player.hp >= player.maxHp) {
            AddLog("Tu salud ya esta al maximo!");
            return;
        }

        const int healed = player.UsePotion();
        combatPres.TriggerPlayerHeal(healed);
        AddLog(player.name + " bebe una pocion.");
        AddLog("Recupera " + std::to_string(healed) + " pts de vida!");

        phase = CombatPhase::ENEMY_WAIT;
        timer = 1.0f;
        break;
    }

    case PlayerAction::FLEE: {
        const int fleeChance = player.GetFleeChance();
        const bool success = (GetRandomValue(1, 100) <= fleeChance);

        if (success) {
            combatPres.Reset();
            AddLog(player.name + " logro escapar!");
            result = CombatResult::FLED;
            phase = CombatPhase::END_WAIT;
            timer = 1.0f;
        }
        else {
            AddLog(player.name + " intento huir pero fallo!");
            phase = CombatPhase::ENEMY_WAIT;
            timer = 1.0f;
        }
        break;
    }

    default:
        break;
    }
}

void Game::EnemyTurn() {
    if (currentEnemy == nullptr) {
        return;
    }

    const int dmg = currentEnemy->CalculateDamage(player);
    player.TakeDamage(dmg);
    combatPres.TriggerEnemyAttack(currentEnemy->isBoss, dmg);
    AddLog(currentEnemy->name + " golpea causando " + std::to_string(dmg) + " pts!");

    if (!player.IsAlive()) {
        CheckCombatEnd();
    }
    else {
        phase = CombatPhase::PLAYER_TURN;
    }
}

void Game::CheckCombatEnd() {
    if (!player.IsAlive()) {
        result = CombatResult::PLAYER_LOST;
        phase = CombatPhase::END_WAIT;
        timer = 1.5f;
    }
    else if (currentEnemy != nullptr && !currentEnemy->IsAlive()) {
        result = CombatResult::PLAYER_WON;
        phase = CombatPhase::END_WAIT;

        if (!combatRewardClaimed) {
            combatRewardClaimed = true;
            AddLog(currentEnemy->name + " ha sido derrotado!");

            const int expEarned = currentEnemy->expReward;
            const ExpResult expRes = player.GainExp(expEarned);
            AddLog("Obtienes " + std::to_string(expEarned) + " pts de EXP.");

            if (expRes.leveledUp) {
                AddLog("SUBIDA DE NIVEL! Ahora eres Nv." + std::to_string(player.level) + "!");
                AddLog("+" + std::to_string(expRes.totalGains.maxHp) + " HP max, +" +
                       std::to_string(expRes.totalGains.maxMp) + " MP max, +" +
                       std::to_string(expRes.totalGains.atk) + " ATK, +" +
                       std::to_string(expRes.totalGains.def) + " DEF");
                timer = 3.0f;
            }
            else {
                timer = 2.0f;
            }

            if (currentEnemy->isBoss) {
                timer = 3.5f;
            }
        }
    }
}

void Game::FinishCombat() {
    switch (result) {
    case CombatResult::PLAYER_WON:
        if (currentEnemy != nullptr) {
            currentEnemy->defeated = true;

            // Desbloqueo de compuertas
            if (currentEnemy->unlocksWall >= 0) {
                map.OpenGate(currentEnemy->unlocksWall);
            }

            if (currentEnemy->isBoss) {
                state = GameState::VICTORY;
                return;
            }
        }

        // En v2: la EXP y subida de nivel ya ocurrieron en CheckCombatEnd.
        // Se mantiene la persistencia estricta de HP y MP (sin restauracion automatica).
        state = GameState::MAP;
        mapCooldown = 0.5f;
        break;

    case CombatResult::PLAYER_LOST:
        state = GameState::GAME_OVER;
        break;

    case CombatResult::FLED: {
        state = GameState::MAP;
        mapCooldown = 2.0f;

        if (currentEnemy != nullptr) {
            const Vector2 pc = player.Center();
            const Vector2 ec = currentEnemy->Center();
            Vector2 away{ pc.x - ec.x, pc.y - ec.y };
            const float len = std::sqrt(away.x * away.x + away.y * away.y);

            if (len > 0.0f) {
                away.x = (away.x / len) * 64.0f;
                away.y = (away.y / len) * 64.0f;
                map.Move(player.body, away);
            }
        }
        UpdateCamera();
        break;
    }

    case CombatResult::NONE:
    default:
        state = GameState::MAP;
        break;
    }

    currentEnemy = nullptr;
}

// ------------------------------------------------------------
// DIBUJO
// ------------------------------------------------------------

void Game::DrawScene(GameState scene) {
    const Vector2 mouse = GetMousePosition();

    switch (scene) {
    case GameState::TITLE_SCREEN: {
        const int hovered = titleView.GetTitleButton(mouse);
        titleView.DrawTitle(titleSel, continueAvailable, hovered);
        break;
    }

    case GameState::CLASS_SELECT: {
        const int hovered = titleView.GetClassCard(mouse);
        const bool backHover = titleView.IsOverBackButton(mouse, GameState::CLASS_SELECT);
        const bool startHover = titleView.IsOverStartButton(mouse);
        titleView.DrawClassSelect(classSel, hovered, backHover, startHover);
        break;
    }

    case GameState::CONTROLS: {
        const bool backHover = titleView.IsOverBackButton(mouse, GameState::CONTROLS);
        titleView.DrawControls(backHover);
        break;
    }

    case GameState::MAP:
        mapView.DrawWorld(map, player, enemies, npcs, camera, nearbyNpc);

        if (showMinimap) {
            mapView.DrawMinimap(map, player, enemies, camera);
        }

        mapView.DrawHUD(player);
        break;

    case GameState::DIALOGUE:
        mapView.DrawWorld(map, player, enemies, npcs, camera, nullptr);

        if (showMinimap) {
            mapView.DrawMinimap(map, player, enemies, camera);
        }

        mapView.DrawHUD(player);

        if (activeNpc != nullptr) {
            const Vector2 mouse = GetMousePosition();
            const int hovered = isDialogueConfirmation ? dialogueView.GetHoveredChoice(mouse) : -1;
            dialogueView.Draw(*activeNpc, currentDialogue, dialogueIndex, isDialogueConfirmation, dialogueChoice, hovered);
        }
        break;

    case GameState::COMBAT:
        if (currentEnemy != nullptr) {
            const int hovered = (phase == CombatPhase::PLAYER_TURN)
                ? ui.GetHoveredButton(mouse)
                : -1;

            ui.Draw(
                player,
                *currentEnemy,
                combatLog,
                selectedButton,
                phase == CombatPhase::PLAYER_TURN,
                hovered,
                combatPres
            );
        }
        break;

    case GameState::GAME_OVER:
        DrawEndScreen("GAME OVER", RED);
        break;

    case GameState::VICTORY:
        DrawEndScreen("VICTORIA!", GOLD);
        break;

    case GameState::PAUSED:
    default:
        break;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    if (state == GameState::PAUSED) {
        DrawScene(prevState);
        mapView.DrawPause(pauseSel);
        if (confirmExit) {
            DrawConfirmExit();
        }
    }
    else {
        DrawScene(state);
    }

    EndDrawing();
}

void Game::DrawEndScreen(const char* title, Color c) const {
    DrawRectangle(0, 0, 960, 540, Fade(BLACK, 0.7f));
    const int titleWidth = MeasureText(title, 60);
    DrawText(title, (960 - titleWidth) / 2, 200, 60, c);

    const char* sub = "[ENTER] Reintentar con el mismo heroe   |   [ESC] Volver al titulo";
    const int subWidth = MeasureText(sub, 20);
    DrawText(sub, (960 - subWidth) / 2, 290, 20, RAYWHITE);
}

void Game::DrawConfirmExit() const {
    DrawRectangle(0, 0, 960, 540, Fade(BLACK, 0.75f));

    Rectangle dialog{ 230.0f, 180.0f, 500.0f, 180.0f };
    DrawRectangleRec(dialog, Color{ 24, 32, 48, 255 });
    DrawRectangleLinesEx(dialog, 3.0f, GOLD);

    const char* q = "Volver a la pantalla de titulo?";
    int qw = MeasureText(q, 24);
    DrawText(q, static_cast<int>(dialog.x + (dialog.width - qw) / 2), static_cast<int>(dialog.y + 28), 24, RAYWHITE);

    const char* sub = "Se perdera cualquier progreso no guardado.";
    int sw = MeasureText(sub, 16);
    DrawText(sub, static_cast<int>(dialog.x + (dialog.width - sw) / 2), static_cast<int>(dialog.y + 68), 16, { 200, 210, 225, 255 });

    const char* opt = "[S / ENTER] Si, volver     [N / ESC] Cancelar";
    int ow = MeasureText(opt, 16);
    DrawText(opt, static_cast<int>(dialog.x + (dialog.width - ow) / 2), static_cast<int>(dialog.y + 120), 16, GOLD);
}
