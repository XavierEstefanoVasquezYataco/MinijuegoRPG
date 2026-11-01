#include "Game.h"

#include <cmath>
#include <string>

Game::Game() {
    InitWindow(SCREEN_W, SCREEN_H, "Mini Aventura RPG");

    // ESC se reserva para pausar, no para cerrar directamente la ventana.
    SetExitKey(KEY_NULL);

    SetTargetFPS(60);
    SetRandomSeed(static_cast<unsigned int>(GetTime() * 1000));

    mapView.Load();
    InitWorld();
}

Game::~Game() {
    mapView.Unload();
    CloseWindow();
}

void Game::InitWorld() {
    map = TileMap();
    player = Player();
    enemies.clear();

    // Lee las marcas A-F del mapa y crea cada enemigo en esa celda.
    const std::vector<TileMap::Spawn> spawns = map.ExtractSpawns();

    for (const TileMap::Spawn& spawn : spawns) {
        const MonsterSpecies* species = FindSpecies(spawn.id);

        if (species != nullptr) {
            enemies.emplace_back(*species, spawn.col, spawn.row);
        }
    }

    state = GameState::MAP;
    prevState = GameState::MAP;
    phase = CombatPhase::PLAYER_TURN;
    result = CombatResult::NONE;

    currentEnemy = nullptr;
    combatLog.clear();

    selectedButton = 0;
    pauseSel = 0;
    timer = 0.0f;
    mapCooldown = 0.0f;
    showMinimap = true;
    quit = false;
}

void Game::Reset() {
    InitWorld();
}

void Game::Run() {
    while (!WindowShouldClose() && !quit) {
        const float dt = GetFrameTime();

        Update(dt);

        BeginDrawing();
        Draw();
        EndDrawing();
    }
}

// ------------------------------------------------------------
// PAUSA
// ------------------------------------------------------------

void Game::Pause() {
    prevState = state;
    state = GameState::PAUSED;
    pauseSel = 0;
}

void Game::Resume() {
    state = prevState;
}

void Game::ActivatePause(int option) {
    switch (option) {
    case 0:
        Resume();
        break;

    case 1:
        Reset();
        break;

    case 2:
        quit = true;
        break;

    default:
        break;
    }
}

void Game::UpdatePause() {
    // Al no llamar UpdateMap ni UpdateCombat, el juego queda congelado.
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
// ACTUALIZACIÓN GENERAL
// ------------------------------------------------------------

void Game::Update(float dt) {
    switch (state) {
    case GameState::MAP:
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
            Pause();
            break;
        }

        UpdateMap(dt);
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
        if (IsKeyPressed(KEY_ENTER)) {
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

    // Actualiza movimiento del jugador y resuelve colisiones con tiles.
    player.Update(dt, map);

    // Actualiza la IA simple de los enemigos.
    for (Enemy& enemy : enemies) {
        enemy.UpdateAI(dt, player.Center(), map);
    }

    // RF-03: tocar a un enemigo inicia el combate.
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

    combatLog.clear();
    AddLog("Un " + enemy->name + " salvaje aparece!");
    AddLog("Tu turno.");
}

void Game::AddLog(const std::string& message) {
    combatLog.push_back(message);

    while (combatLog.size() > 6) {
        combatLog.pop_front();
    }
}

void Game::UpdateCombat(float dt) {
    switch (phase) {
    case CombatPhase::PLAYER_TURN: {
        // Menú 2 x 2:
        // 0 Atacar, 1 Habilidad, 2 Poción, 3 Huir.
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

        const PlayerAction clickedAction =
            ui.GetClickedAction(GetMousePosition());

        if (clickedAction != PlayerAction::NONE) {
            selectedButton = static_cast<int>(clickedAction);
            action = clickedAction;
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

    Enemy& enemy = *currentEnemy;
    bool turnConsumed = true;

    switch (action) {
    case PlayerAction::ATTACK: {
        const int damage = player.CalculateDamage(enemy);
        enemy.TakeDamage(damage);

        AddLog("Atacas: " + std::to_string(damage) + " de dano.");
        break;
    }

    case PlayerAction::SKILL: {
        // Si no hay MP suficiente, el jugador conserva su turno.
        if (!player.UseSkillCost()) {
            AddLog(
                "MP insuficiente! (" +
                std::to_string(Player::SKILL_COST) +
                " MP)"
            );

            turnConsumed = false;
            break;
        }

        const int damage =
            player.CalculateDamage(enemy, Player::SKILL_MULT);

        enemy.TakeDamage(damage);

        AddLog(
            "Habilidad: " +
            std::to_string(damage) +
            " de dano."
        );

        break;
    }

    case PlayerAction::POTION: {
        if (!player.UsePotion()) {
            AddLog("No te quedan pociones!");
            turnConsumed = false;
            break;
        }

        AddLog(
            "Pocion: +" +
            std::to_string(Player::POTION_HEAL) +
            " HP."
        );

        break;
    }

    case PlayerAction::FLEE: {
        // Probabilidad de huida del 50 %.
        if (GetRandomValue(1, 100) <= 50) {
            AddLog("Lograste huir!");

            result = CombatResult::FLED;
            phase = CombatPhase::END_WAIT;
            timer = 1.0f;
            return;
        }

        AddLog("No pudiste huir!");
        break;
    }

    default:
        return;
    }

    // Acciones no válidas, como usar habilidad sin MP, no consumen turno.
    if (!turnConsumed) {
        return;
    }

    if (!enemy.IsAlive()) {
        CheckCombatEnd();
        return;
    }

    // El enemigo actuará después de una pausa breve.
    phase = CombatPhase::ENEMY_WAIT;
    timer = 1.0f;
}

void Game::EnemyTurn() {
    if (currentEnemy == nullptr) {
        return;
    }

    const int damage = currentEnemy->CalculateDamage(player);
    player.TakeDamage(damage);

    AddLog(
        currentEnemy->name +
        " ataca: " +
        std::to_string(damage) +
        " de dano."
    );

    if (!player.IsAlive()) {
        CheckCombatEnd();
        return;
    }

    phase = CombatPhase::PLAYER_TURN;
    AddLog("Tu turno.");
}

void Game::CheckCombatEnd() {
    if (currentEnemy == nullptr) {
        return;
    }

    if (!player.IsAlive()) {
        result = CombatResult::PLAYER_LOST;
        AddLog("Has sido derrotado...");
    }
    else if (!currentEnemy->IsAlive()) {
        result = CombatResult::PLAYER_WON;
        AddLog(currentEnemy->name + " derrotado!");
    }
    else {
        return;
    }

    phase = CombatPhase::END_WAIT;
    timer = 1.5f;
}

void Game::FinishCombat() {
    if (currentEnemy == nullptr) {
        state = GameState::MAP;
        return;
    }

    switch (result) {
    case CombatResult::PLAYER_LOST:
        state = GameState::GAME_OVER;
        break;

    case CombatResult::PLAYER_WON:
        currentEnemy->defeated = true;

        player.LevelUp();
        player.FullRestore();

        // Abre la compuerta asociada a este monstruo, si tiene una.
        if (currentEnemy->unlocksWall >= 0) {
            map.OpenGate(currentEnemy->unlocksWall);
        }

        // Derrotar al jefe lleva a la pantalla de victoria.
        if (currentEnemy->isBoss) {
            state = GameState::VICTORY;
        }
        else {
            state = GameState::MAP;
        }

        mapCooldown = 0.5f;
        break;

    case CombatResult::FLED: {
        state = GameState::MAP;
        mapCooldown = 2.0f;

        // Empuja un poco al jugador alejándolo del enemigo.
        const Vector2 playerCenter = player.Center();
        const Vector2 enemyCenter = currentEnemy->Center();

        const float dx = playerCenter.x - enemyCenter.x;
        const float dy = playerCenter.y - enemyCenter.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 0.0f) {
            map.Move(
                player.body,
                {
                    dx / distance * 64.0f,
                    dy / distance * 64.0f
                }
            );
        }

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
    switch (scene) {
    case GameState::MAP:
        mapView.DrawWorld(map, player, enemies);

        if (showMinimap) {
            mapView.DrawMinimap(map, player, enemies);
        }

        mapView.DrawHUD(player);
        break;

    case GameState::COMBAT:
        if (currentEnemy != nullptr) {
            ui.Draw(
                player,
                *currentEnemy,
                combatLog,
                selectedButton,
                phase == CombatPhase::PLAYER_TURN
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
    if (state == GameState::PAUSED) {
        // Dibuja la escena anterior sin actualizarla.
        DrawScene(prevState);
        mapView.DrawPause(pauseSel);
    }
    else {
        DrawScene(state);
    }
}

void Game::DrawEndScreen(const char* title, Color color) const {
    ClearBackground(BLACK);

    const int titleWidth = MeasureText(title, 60);

    DrawText(
        title,
        (SCREEN_W - titleWidth) / 2,
        200,
        60,
        color
    );

    const char* hint = "Presiona ENTER para reiniciar";
    const int hintWidth = MeasureText(hint, 24);

    DrawText(
        hint,
        (SCREEN_W - hintWidth) / 2,
        300,
        24,
        RAYWHITE
    );
}