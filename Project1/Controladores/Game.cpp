/**
 * @file Game.cpp
 * @brief Implementación del orquestador principal del juego, máquina de estados y ciclo de eventos.
 *
 * Capa de Arquitectura: Controlador (Controller) - Orquestador Central.
 *
 * Propósito y Responsabilidades:
 * - Implementar el bucle principal de ejecución (`Run`).
 * - Procesar las entradas de teclado y ratón desacoplando el hardware de los modelos de datos.
 * - Coordinar la máquina de estados de juego (Título, Selección de Héroe, Controles, Mapa, Diálogo, Combate, Pausa).
 * - Restringir suavemente el encuadre de la cámara 2D (*camera clamping*) para confinar la vista al mundo de 1920x1024 px.
 * - Resolver las reglas del combate por turnos, turnos enemigos y subidas escalonadas de nivel.
 */

#include "Game.h"
#include <algorithm>
#include <cmath>
#include <string>
#include "raylib.h"

namespace {
    MusicTrack TrackForState(GameState s) {
        switch (s) {
        case GameState::MAP:
        case GameState::DIALOGUE:
            return MusicTrack::MAP_THEME;
        case GameState::COMBAT:
            return MusicTrack::COMBAT_THEME;
        default:
            return MusicTrack::MENU_THEME;
        }
    }
}

// ------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR / BUCLE PRINCIPAL
// ------------------------------------------------------------

/**
 * @brief Constructor principal del juego.
 *
 * Acciones ejecutadas:
 * 1. Inicializa la ventana de Raylib a 960x540 píxeles con el título "Mini Aventura RPG".
 * 2. Fija la tasa de refresco a 60 FPS estables.
 * 3. Ejecuta `SetExitKey(KEY_NULL)` para deshabilitar el cierre automático por defecto de Raylib al pulsar ESCAPE,
 *    permitiendo que la tecla Escape se use limpiamente para retroceder menús o abrir la pausa.
 * 4. Carga las texturas auxiliares en `MapView` y genera el mundo inicial en `TITLE_SCREEN`.
 */
Game::Game() {
    InitWindow(SCREEN_W, SCREEN_H, "Mini Aventura RPG");
    SetTargetFPS(60);

    // Desactivar el cierre automático de Raylib por tecla ESCAPE
    // para centralizar el comportamiento de Escape en el controlador
    SetExitKey(KEY_NULL);

    audio.Init();
    mapView.Load();
    InitWorld();
    state = GameState::TITLE_SCREEN;
}

/**
 * @brief Destructor. Descarga texturas de la GPU y cierra la ventana multimedia.
 */
Game::~Game() {
    mapView.Unload();
    audio.Shutdown();
    CloseWindow();
}

/**
 * @brief Inicializa o restablece todos los elementos del mundo de juego.
 *
 * - Reconstruye la matriz del `TileMap` y extrae los puntos de generación ('A'..'F') para poblar los enemigos.
 * - Restablece al héroe en su posición inicial de la aldea segura.
 * - Genera los NPCs y carteles del mapa.
 * - Configura la cámara 2D en offset de pantalla media ({ 480, 270 }) con zoom 1.0f.
 */
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

    // Configuración de la cámara 2D centrada en el jugador
    camera.offset = { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    UpdateCamera();
}

/**
 * @brief Reinicia el estado del mundo y limpia los efectos visuales de combate.
 */
void Game::Reset() {
    InitWorld();
    combatPres.Reset();
}

/**
 * @brief Restringe suavemente el objetivo de la cámara 2D dentro de los bordes del mapa (Clamping).
 *
 * Para evitar que el jugador vea el espacio vacío negro exterior a la cuadrícula:
 * - Calcula la mitad del ancho (halfW = 480 px) y alto (halfH = 270 px) de la ventana virtual (960x540 px).
 * - Calcula los límites mundiales totales: ancho = 60 cols * 32 px = 1920 px; alto = 32 rows * 32 px = 1024 px.
 * - Aplica `std::clamp` sobre `camera.target.x` entre `halfW` y `anchoMundo - halfW`.
 * - Aplica `std::clamp` sobre `camera.target.y` entre `halfH` y `altoMundo - halfH`.
 *
 * Ejemplo numérico de restricción de cámara:
 * - Rango permitido para camera.target.x: [480.0f .. (1920.0f - 480.0f = 1440.0f)].
 * - Rango permitido para camera.target.y: [270.0f .. (1024.0f - 270.0f = 754.0f)].
 * - Si el héroe se encuentra en la aldea segura en X = 120 px, el clamp fija camera.target.x en 480 px,
 *   evitando que la ventana encuadre el vacío negro a la izquierda del mapa.
 * - Si el héroe camina al sanctum final en X = 1800 px, camera.target.x se restringe a 1440 px.
 */
void Game::UpdateCamera() {
    const Vector2 target = player.Center();
    const float halfW = SCREEN_W / 2.0f;
    const float halfH = SCREEN_H / 2.0f;
    const float mapPixelW = static_cast<float>(TileMap::COLS * TileMap::TILE);
    const float mapPixelH = static_cast<float>(TileMap::ROWS * TileMap::TILE);

    camera.target.x = std::clamp(target.x, halfW, mapPixelW - halfW);
    camera.target.y = std::clamp(target.y, halfH, mapPixelH - halfH);
}

/**
 * @brief Inicia una nueva partida con el arquetipo de héroe seleccionado por el usuario.
 * @param chosenClass Arquetipo elegido (Guerrero, Maga, Exploradora).
 */
void Game::StartNewGame(HeroClass chosenClass) {
    Reset();
    player.InitClass(chosenClass);
    UpdateCamera();
    state = GameState::MAP;
}

/**
 * @brief Bucle central de ejecución de la aplicación a 60 cuadros por segundo.
 */
void Game::Run() {
    while (!WindowShouldClose() && !quit) {
        const float dt = GetFrameTime();
        Update(dt);
        Draw();
    }
}

// ------------------------------------------------------------
// GESTIÓN DE PAUSA
// ------------------------------------------------------------

/**
 * @brief Congela el estado actual del juego y abre la ventana de pausa superpuesta.
 */
void Game::Pause() {
    prevState = state;
    state = GameState::PAUSED;
    pauseSel = 0;
    confirmExit = false;
    saveMsgTimer = 0.0f;
}

/**
 * @brief Reanuda la partida restaurando el estado previo a la pausa.
 */
void Game::Resume() {
    state = prevState;
    confirmExit = false;
}

/**
 * @brief Ejecuta la acción seleccionada en el menú de pausa.
 * @param option 0 para Reanudar, 1 para Guardar partida, 2 para Reiniciar con el mismo héroe, 3 para Volver al Título.
 */
void Game::ActivatePause(int option) {
    switch (option) {
    case 0: // Reanudar
        Resume();
        break;
    case 1:
        if (prevState == GameState::MAP) {
            saveMsg = SaveGame() ? "Partida guardada" : "No se pudo guardar la partida";
        }
        else {
            saveMsg = "Solo se puede guardar en el mapa";
        }
        saveMsgTimer = 2.5f;
        break;
    case 2: { // Reiniciar (conserva la clase elegida)
        const HeroClass currentClass = player.heroClass;
        Reset();
        player.InitClass(currentClass);
        Resume();
        break;
    }
    case 3: // Volver al título (solicita confirmación para evitar pérdidas accidentales)
        confirmExit = true;
        break;
    default:
        break;
    }
}

/**
 * @brief Actualiza la interacción por teclado y ratón durante el estado de pausa.
 */
void Game::UpdatePause() {
    // Si la ventana de confirmación de salida está activa
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

    // Navegación vertical por opciones
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        pauseSel = (pauseSel + 1) % 4;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        pauseSel = (pauseSel + 3) % 4;
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
// MENÚ DE TÍTULO, CLASES Y CONTROLES
// ------------------------------------------------------------

/**
 * @brief Procesa la interacción en el menú de título principal.
 */
void Game::UpdateTitle() {
    // Navegación con teclado saltando la opción "Continuar" si no hay guardado disponible
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        titleSel = (titleSel + 1) % 4;
        if (titleSel == 1 && !continueAvailable) {
            titleSel = 2;
        }
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        titleSel = (titleSel + 3) % 4;
        if (titleSel == 1 && !continueAvailable) {
            titleSel = 0;
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
        case 1: // Continuar
            if (continueAvailable && !LoadGame()) {
                continueAvailable = false;
                titleSel = 0;
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

/**
 * @brief Procesa la interacción en la pantalla de selección de héroe.
 */
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

/**
 * @brief Procesa el retorno desde la pantalla informativa de Controles.
 */
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
// ACTUALIZACIÓN GENERAL (MÁQUINA DE ESTADOS)
// ------------------------------------------------------------

/**
 * @brief Deriva la actualización lógica a la función especializada según el estado activo.
 * @param dt Tiempo transcurrido del fotograma en segundos.
 */
void Game::Update(float dt) {
    if (state != lastState) {
        if (state == GameState::TITLE_SCREEN) {
            continueAvailable = SaveSystem::Exists();
        }
        lastState = state;
    }

    if (saveMsgTimer > 0.0f) {
        saveMsgTimer -= dt;
    }

    audio.Play(TrackForState(state == GameState::PAUSED ? prevState : state));
    audio.Update();

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
            const HeroClass currentClass = player.heroClass;
            Reset();
            player.InitClass(currentClass);
            UpdateCamera();
            state = GameState::MAP;
        }
        else if (IsKeyPressed(KEY_ESCAPE)) {
            state = GameState::TITLE_SCREEN;
            Reset();
        }
        break;
    }
}

// ------------------------------------------------------------
// MODO SUPRAMUNDO (EXPLORACIÓN Y COLISIONES)
// ------------------------------------------------------------

/**
 * @brief Procesa la exploración, lectura de controles de movimiento, colisiones y detección de interacciones.
 *
 * Flujo:
 * 1. Alterna el minimapa al presionar `M`.
 * 2. Lee teclas WASD/Flechas en el controlador y calcula el desplazamiento deseado en `Player`.
 * 3. Evalúa colisiones independientes en X e Y contra el `TileMap` y personajes sólidos (`NPC`).
 * 4. Actualiza la posición de la cámara centrada en el héroe con clamping.
 * 5. Escanea NPCs en radio de interacción (46 px). Si el usuario presiona `E`, inicia el diálogo.
 * 6. Actualiza la IA de enemigos y verifica si alguno hace contacto físico con el jugador para iniciar combate.
 */
void Game::UpdateMap(float dt) {
    if (IsKeyPressed(KEY_M)) {
        showMinimap = !showMinimap;
    }

    if (mapCooldown > 0.0f) {
        mapCooldown -= dt;
    }

    // Lectura de entrada de hardware centralizada en el Controlador (MVC)
    Vector2 inputDir{ 0.0f, 0.0f };
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    inputDir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  inputDir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  inputDir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) inputDir.x += 1.0f;

    // Desplazamiento y orientación del jugador
    Vector2 m = player.GetDesiredMove(inputDir, dt);
    player.moving = (m.x != 0.0f || m.y != 0.0f);
    if (player.moving) {
        if (std::fabs(m.x) > std::fabs(m.y)) player.facing = (m.x < 0) ? Facing::LEFT : Facing::RIGHT;
        else                                 player.facing = (m.y < 0) ? Facing::UP : Facing::DOWN;
        player.animTimer += dt;
    } else {
        player.animTimer = 0.0f;
    }

    // Movimiento en X con resolución de colisiones contra tiles y personajes físicos
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

    // Movimiento en Y con resolución de colisiones contra tiles y personajes físicos
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

    // Actualiza la posición de la cámara de seguimiento con clamping
    UpdateCamera();

    // Detección determinista del elemento interactuable más cercano (radio 46 px)
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

    // Actualiza la IA simple de los enemigos
    for (Enemy& enemy : enemies) {
        enemy.UpdateAI(dt, player.Center(), map);
    }

    // Contacto físico con un enemigo inicia combate por turnos
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
// DIÁLOGOS E INTERACCIÓN CON NPCS
// ------------------------------------------------------------

/**
 * @brief Inicia una conversación activa con un NPC o la lectura de un cartel.
 *
 * Evalúa el progreso del juego consultando el estado de compuertas y jefe en el modelo.
 */
void Game::StartDialogue(const NPC& npc) {
    activeNpc = &npc;

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
    dialogueChoice = 0; // Por defecto: Sí
    state = GameState::DIALOGUE;
}

/**
 * @brief Gestiona el avance de texto y la respuesta a preguntas de confirmación.
 *
 * En el caso de la Curandera Elena:
 * - Si el jugador confirma (Sí), ejecuta `player.FullRestore()`.
 * - Aplica la regla anti-acumulación de pociones: si posee menos de 2 pociones,
 *   las reabastece a exactamente 2; si ya poseía 2 o más, no altera el inventario.
 */
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
        // Avance de página estándar
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

/**
 * @brief Cierra el cuadro de diálogo y devuelve el control al modo exploración.
 */
void Game::EndDialogue() {
    activeNpc = nullptr;
    dialogueIndex = 0;
    currentDialogue.clear();
    isDialogueConfirmation = false;
    state = GameState::MAP;
}

// ------------------------------------------------------------
// COMBATE POR TURNOS
// ------------------------------------------------------------

/**
 * @brief Inicia un encuentro de combate por turnos contra el enemigo especificado.
 */
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

/**
 * @brief Añade una línea al registro de combate, manteniendo un máximo de 10 mensajes.
 */
void Game::AddLog(const std::string& msg) {
    combatLog.push_back(msg);
    while (combatLog.size() > 10) {
        combatLog.pop_front();
    }
}

/**
 * @brief Máquina de sub-estados para el combate por turnos (Turno Jugador, Espera Enemigo, Espera Fin).
 */
void Game::UpdateCombat(float dt) {
    if (currentEnemy != nullptr) {
        combatPres.Update(dt, player, *currentEnemy);
    }

    switch (phase) {
    case CombatPhase::PLAYER_TURN: {
        // Navegación en grilla 2x2 mediante álgebra booleana (XOR bitwise):
        // Distribución:
        // [0: ATACAR (00b)]      [1: HABILIDAD (01b)]
        // [2: POCION (10b)]      [3: HUIR (11b)]
        // - Movimiento horizontal (Izquierda / Derecha o A / D):
        //   selectedButton ^= 1 invierte el bit 0: alternando entre 0 <-> 1 y entre 2 <-> 3.
        // - Movimiento vertical (Arriba / Abajo o W / S):
        //   selectedButton ^= 2 invierte el bit 1: alternando entre 0 <-> 2 y entre 1 <-> 3.
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

/**
 * @brief Ejecuta el comando seleccionado por el jugador (Atacar, Habilidad, Poción, Huir).
 */
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

/**
 * @brief Ejecuta el contraataque del enemigo contra el héroe.
 */
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

/**
 * @brief Comprueba condiciones de victoria o derrota al concluir un ataque.
 *
 * Si el jugador vence:
 * - Acredita los puntos de EXP del enemigo llamando a `player.GainExp()`.
 * - Informa si se produjeron subidas de nivel y reporta las ganancias de estadísticas en el log.
 */
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

/**
 * @brief Aplica las consecuencias definitivas del combate y realiza la transición de estado.
 *
 * - Si ganó: marca al enemigo como derrotado, abre compuertas asociadas y si es jefe transiciona a `VICTORY`.
 * - Si perdió: transiciona a `GAME_OVER`.
 * - Si huyó: empuja al héroe 64 píxeles en dirección opuesta al enemigo y otorga 2 segundos de inmunidad.
 */
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

        // Persistencia estricta: conserva HP y MP resultantes de la pelea
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
// GUARDADO Y CARGA DE PARTIDA
// ------------------------------------------------------------

bool Game::SaveGame() const {
    SaveData data;
    data.heroClass = static_cast<int>(player.heroClass);
    data.level = player.level;
    data.hp = player.hp;
    data.maxHp = player.maxHp;
    data.mp = player.mp;
    data.maxMp = player.maxMp;
    data.atk = player.atk;
    data.def = player.def;
    data.potions = player.potions;
    data.exp = player.exp;
    data.expNext = player.expNext;
    data.posX = player.body.x;
    data.posY = player.body.y;
    data.facing = static_cast<int>(player.facing);
    data.showMinimap = showMinimap;

    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].defeated) {
            data.defeatedEnemies.push_back(static_cast<int>(i));
        }
    }

    return SaveSystem::Save(data);
}

bool Game::LoadGame() {
    SaveData data;
    if (!SaveSystem::Load(data)) {
        return false;
    }

    Reset();
    player.InitClass(static_cast<HeroClass>(data.heroClass));
    player.level = data.level;
    player.hp = data.hp;
    player.maxHp = data.maxHp;
    player.mp = data.mp;
    player.maxMp = data.maxMp;
    player.atk = data.atk;
    player.def = data.def;
    player.potions = data.potions;
    player.exp = data.exp;
    player.expNext = data.expNext;
    player.facing = static_cast<Facing>(data.facing);
    player.moving = false;

    const float maxX = static_cast<float>(TileMap::COLS * TileMap::TILE) - player.body.width;
    const float maxY = static_cast<float>(TileMap::ROWS * TileMap::TILE) - player.body.height;
    player.body.x = std::clamp(data.posX, 0.0f, maxX);
    player.body.y = std::clamp(data.posY, 0.0f, maxY);

    showMinimap = data.showMinimap;

    for (int index : data.defeatedEnemies) {
        if (index >= 0 && index < static_cast<int>(enemies.size())) {
            Enemy& enemy = enemies[index];
            enemy.defeated = true;
            if (enemy.unlocksWall >= 0) {
                map.OpenGate(enemy.unlocksWall);
            }
        }
    }

    UpdateCamera();
    state = GameState::MAP;
    return true;
}

// ------------------------------------------------------------
// RENDERIZADO GENERAL Y PANTALLAS FINALES
// ------------------------------------------------------------

/**
 * @brief Delega el renderizado de la escena visual a la vista pasiva correspondiente.
 */
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
            const Vector2 mousePos = GetMousePosition();
            const int hovered = isDialogueConfirmation ? dialogueView.GetHoveredChoice(mousePos) : -1;
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

/**
 * @brief Ciclo de dibujado: inicia `BeginDrawing`, limpia fondo, renderiza escena y finaliza `EndDrawing`.
 */
void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    if (state == GameState::PAUSED) {
        // Dibuja la escena de fondo congelada antes de superponer la pausa
        DrawScene(prevState);
        mapView.DrawPause(pauseSel);
        if (saveMsgTimer > 0.0f && !confirmExit) {
            const int msgWidth = MeasureText(saveMsg, 22);
            DrawText(saveMsg, (SCREEN_W - msgWidth) / 2, 478, 22, GOLD);
        }
        if (confirmExit) {
            DrawConfirmExit();
        }
    }
    else {
        DrawScene(state);
    }

    EndDrawing();
}

/**
 * @brief Renderiza pantallas de desenlace (Game Over o Victoria).
 */
void Game::DrawEndScreen(const char* title, Color c) const {
    DrawRectangle(0, 0, 960, 540, Fade(BLACK, 0.7f));
    const int titleWidth = MeasureText(title, 60);
    DrawText(title, (960 - titleWidth) / 2, 200, 60, c);

    const char* sub = "[ENTER] Reintentar con el mismo heroe   |   [ESC] Volver al titulo";
    const int subWidth = MeasureText(sub, 20);
    DrawText(sub, (960 - subWidth) / 2, 290, 20, RAYWHITE);
}

/**
 * @brief Renderiza el cuadro de diálogo de confirmación para salir a la pantalla de título.
 */
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
