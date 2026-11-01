# INFORME TÉCNICO DEL PROYECTO ACTUAL (Mini Aventura RPG)

---

## 1. IDENTIFICACIÓN Y ESTRUCTURA

### 1.1. Nombre del proyecto y objetivo del juego
* **Nombre de la solución / proyecto:** `Project1` (definido en [Project1.slnx](file:///C:/Users/Xavier/source/repos/Project1/Project1.slnx#L6) y [Project1.vcxproj](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj#L25)).
* **Título de la ventana:** `"Mini Aventura RPG"` (configurado en [Game.cpp:7](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L7)).
* **Objetivo del juego:** Es un prototipo funcional de RPG en 2D con vista cenital (*top-down*) y combate clásico por turnos. El jugador controla a un Héroe que debe explorar un supramundo dividido en tres áreas separadas por compuertas numeradas (`'1'` y `'2'`). Para progresar, el jugador debe encontrar y derrotar a monstruos específicos que actúan como guardianes para abrir el paso hasta llegar a la guarida del Dragón (jefe final). Al derrotarlo, se activa la pantalla de victoria.

### 1.2. Árbol de carpetas y archivos relevantes
```
C:\Users\Xavier\source\repos\Project1\
├── Project1.slnx                           # Solución Visual Studio (formato XML moderno .slnx)
├── packages.config                         # Configuración de paquete NuGet raíz
├── INFORME_PROYECTO_ACTUAL.md              # Este informe técnico
├── packages\                               # Paquetes NuGet descargados
│   └── raylib.5.5.0\                       # Librería Raylib nativa (include, lib x64/win32)
├── x64\Debug\                              # Salida previa de compilación raíz (Project1.exe, pdb)
└── Project1\                               # Directorio principal del código fuente
    ├── main.cpp                            # Punto de entrada de la aplicación
    ├── GameState.h                         # Enumeraciones de estado y tipos globales
    ├── Game.h / Game.cpp                   # Bucle principal, control de estados y reglas de combate
    ├── Player.h / Player.cpp               # Clases Combatant, Player, Enemy y tabla MonsterSpecies
    ├── TileMap.h / TileMap.cpp             # Lógica de matriz de celdas, colisiones y compuertas
    ├── MapView.h / MapView.cpp             # Renderizado del mapa, HUD, minimapa y pausa
    ├── CombatUI.h / CombatUI.cpp           # Renderizado de arena de combate y control de menú
    ├── PixelArt.h / PixelArt.cpp           # Rasterizador procedimental de sprites 12x12
    ├── packages.config                     # Configuración NuGet del proyecto secundario
    ├── Project1.vcxproj                    # Archivo de proyecto C++ MSBuild
    ├── Project1.vcxproj.filters            # Estructura de filtros lógicos en Visual Studio
    ├── Project1.vcxproj.user               # Preferencias locales del depurador
    └── x64\Debug\                          # Archivos objeto (.obj), ejecutables y registros locales
```

### 1.3. Solución .sln, proyectos .vcxproj y archivos de configuración
* **Solución:** [Project1.slnx](file:///C:/Users/Xavier/source/repos/Project1/Project1.slnx) (utiliza el esquema simplificado XML de Visual Studio 2022 v18/v17, conteniendo únicamente la referencia a `Project1/Project1.vcxproj`).
* **Proyecto:** [Project1.vcxproj](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj) (archivo MSBuild que declara configuraciones para `Debug|Win32`, `Release|Win32`, `Debug|x64`, `Release|x64`).
* **Filtros:** [Project1.vcxproj.filters](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj.filters) (agrupa archivos en `Modelo`, `Vistas`, `Controladores` y `Archivos de origen`).
* **Gestión de paquetes:** [packages.config](file:///C:/Users/Xavier/source/repos/Project1/Project1/packages.config) referencia el paquete `raylib` versión `5.5.0` con destino `native`.

### 1.4. Código fuente activo y versiones antiguas o excluidas
* **Archivos fuente activos:**
  * Archivos `.cpp`: `main.cpp`, `Game.cpp`, `Player.cpp`, `TileMap.cpp`, `MapView.cpp`, `CombatUI.cpp`, `PixelArt.cpp`.
  * Archivos `.h`: `GameState.h`, `Game.h`, `Player.h`, `TileMap.h`, `MapView.h`, `CombatUI.h`, `PixelArt.h`.
* **Archivos antiguos, copias o excluidos:**
  * No existen archivos `.cpp` ni `.h` huérfanos, copias de seguridad (`.bak`, `.old`), ni versiones alternativas dentro del árbol del repositorio. Todos los archivos de código existentes están formalmente incluidos en el archivo [Project1.vcxproj](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj#L144-L161).
  * En la carpeta `C:\Users\Xavier\source\repos\Project1\x64\Debug` existen binarios antiguos (`Project1.exe` y `Project1.pdb`) generados con fecha anterior (9/10/2026), mientras que la compilación activa genera los binarios en `C:\Users\Xavier\source\repos\Project1\Project1\x64\Debug\`.

---

## 2. TECNOLOGÍAS Y CONFIGURACIÓN

### 2.1. Lenguaje, estándar C++ configurado y toolset
* **Lenguaje:** C++.
* **Estándar C++:** **C++20** (`<LanguageStandard>stdcpp20</LanguageStandard>`, configurado en [Project1.vcxproj:79, 97, 113, 131](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj#L79)).
* **Toolset de compilación:** `v145` (Visual Studio 2022 / v18 Insiders, compilador MSVC CL versión `14.51.36231`).
* **Configuración del compilador:** Conformidad estricta (`<ConformanceMode>true</ConformanceMode>` / `/permissive-`), nivel de advertencia 3 (`/W3`), comprobaciones SDL (`/sdl`).

### 2.2. Bibliotecas y versiones verificables
* **Raylib:** Versión **5.5.0** (verificada en [packages.config:3](file:///C:/Users/Xavier/source/repos/Project1/Project1/packages.config#L3) y en `packages/raylib.5.5.0/`).
* **C++ Standard Library (STL de MSVC):** Contenedores y utilidades estándar verificados en código: `<deque>`, `<vector>`, `<string>`, `<cmath>`, `<algorithm>`.
* **Librerías del SDK de Windows enlazadas automáticamente por Raylib:** `winmm.lib`, `kernel32.lib`, `user32.lib`, `gdi32.lib`, `winspool.lib`, `comdlg32.lib`, `advapi32.lib`, `shell32.lib`, `ole32.lib`, `oleaut32.lib`, `uuid.lib`.

### 2.3. Dependencias y forma de instalación
* La dependencia externa `raylib` está empaquetada como un paquete NuGet nativo de C++.
* El archivo `Project1.vcxproj` importa automáticamente el archivo de propiedades y enlace:
  `..\packages\raylib.5.5.0\build\native\raylib.targets` ([Project1.vcxproj:164](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj#L164)).

### 2.4. Plataforma y configuración de compilación
* **Plataformas soportadas en proyecto:** `x64` y `Win32`.
* **Configuraciones:** `Debug` y `Release`.
* **Subsistema:** Consola (`/SUBSYSTEM:CONSOLE`, configurado en [Project1.vcxproj:82, 100, 116, 134](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj#L82)).

### 2.5. Rutas absolutas o configuraciones que dificulten la portabilidad
* **En el archivo de proyecto:** No existen rutas absolutas codificadas en `Project1.vcxproj`. Las rutas a los paquetes NuGet son relativas (`..\packages\...`).
* **En tiempo de ejecución:** En [MapView.cpp:15](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L15) se utiliza la ruta relativa `"assets/hero.png"`. Si el directorio de trabajo (*working directory*) configurado al ejecutar el binario no coincide con la carpeta raíz del proyecto, no encontrará el archivo (aunque cuenta con un mecanismo de respaldo procedimental seguro).
* **Dependencia de NuGet:** Al clonar o mover el proyecto a otra máquina sin la carpeta `packages/`, se requerirá una restauración de paquetes NuGet (`nuget restore` o restauración automática de dependencias de Visual Studio).

---

## 3. ARQUITECTURA Y EJECUCIÓN

### 3.1. Punto de entrada y ciclo principal
* **Punto de entrada:** [main.cpp:3-7](file:///C:/Users/Xavier/source/repos/Project1/Project1/main.cpp#L3-L7):
  ```cpp
  int main() {
      Game game;
      game.Run();
      return 0;
  }
  ```
* **Ciclo principal:** [Game::Run](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L60-L70):
  * Ejecuta un bucle `while (!WindowShouldClose() && !quit)` fijado a 60 FPS (`SetTargetFPS(60)`).
  * Obtiene el delta time del fotograma: `const float dt = GetFrameTime();`.
  * Llama a `Update(dt)`.
  * Encapsula el renderizado entre `BeginDrawing()`, `Draw()` y `EndDrawing()`.

### 3.2. Clases, estructuras y módulos principales
* **`Game`** ([Game.h:12-66](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.h#L12-L66)): Orquestador general del juego. Contiene la instancia del jugador, del mapa, la lista de enemigos, las vistas y gestiona las transiciones entre estados.
* **`TileMap`** ([TileMap.h:8-31](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.h#L8-L31)): Encapsula la cuadrícula de 30x16 celdas, consulta de tipos de celda, resolución de colisiones AABB y desbloqueo de compuertas.
* **`Combatant`** ([Player.h:23-39](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L23-L39)): Clase base de estadísticas de combate (`hp`, `maxHp`, `mp`, `maxMp`, `atk`, `def`), daño y curación.
* **`Player`** ([Player.h:41-68](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L41-L68)): Especialización de `Combatant` que añade posición física (`body`), velocidad, dirección de mirada (`facing`), timer de animación, inventario de pociones y método de subida de nivel (`LevelUp`).
* **`Enemy`** ([Player.h:70-95](file:///C:/Users/Xavier/source/repos/Project1/Player.h#L70-L95)): Especialización de `Combatant` que añade IA de supramundo (`AIState`), anclaje a punto de origen (`home`), radios de detección, colores de renderizado y compuerta asignada.
* **`MonsterSpecies`** ([Player.h:9-18](file:///C:/Users/Xavier/source/repos/Project1/Player.h#L9-L18)): Estructura descriptora de datos fijos para cada especie de criatura.
* **`MapView`** ([MapView.h:7-29](file:///C:/Users/Xavier/source/repos/Project1/MapView.h#L7-L29)): Dibuja el escenario del supramundo, el minimapa en tiempo real, el HUD inferior y la pantalla de pausa.
* **`CombatUI`** ([CombatUI.h:8-25](file:///C:/Users/Xavier/source/repos/Project1/CombatUI.h#L8-L25)): Dibuja la arena de combate por turnos, las cajas de estado (HP/MP), el historial de eventos (*combat log*) y el menú de botones 2x2.
* **`PixelArt`** ([PixelArt.h:1-8](file:///C:/Users/Xavier/source/repos/Project1/PixelArt.h#L1-L8)): Módulo procedimental con la función `DrawPixelArt` que dibuja en pantalla matrices 12x12 escaladas según una paleta predeterminada.

### 3.3. Estados del juego y transiciones
Definidos en [GameState.h:3](file:///C:/Users/Xavier/source/repos/Project1/Project1/GameState.h#L3):
`enum class GameState { MAP, COMBAT, PAUSED, GAME_OVER, VICTORY };`

* **`MAP`:**
  * Transiciona a `COMBAT` al detectar solapamiento rectangular entre `player.body` y cualquier `enemy.body` activo ([Game.cpp:193-201](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L193-L201)), siempre que `mapCooldown <= 0.0f`.
  * Transiciona a `PAUSED` si el usuario presiona las teclas `P` o `ESCAPE` ([Game.cpp:141-144](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L141-L144)).
* **`COMBAT`:**
  * Se subdivide en fases (`CombatPhase`):
    * `PLAYER_TURN`: El usuario selecciona una acción mediante menú o mouse ([Game.cpp:236-272](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L236-L272)).
    * `ENEMY_WAIT`: Pausa de 1.0 segundo tras la cual actúa el enemigo llamando a `EnemyTurn()` ([Game.cpp:274-281](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L274-L281)).
    * `END_WAIT`: Pausa de 1.0 a 1.5 segundos antes de llamar a `FinishCombat()` ([Game.cpp:283-290](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L283-L290)).
  * Transiciona a:
    * `MAP`: Si el enemigo derrotado es estándar, o si el jugador huye con éxito (aplicando empuje de 64px y cooldown de 2s) ([Game.cpp:445-489](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L445-L489)).
    * `VICTORY`: Si el enemigo derrotado tiene `isBoss == true` ([Game.cpp:456-458](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L456-L458)).
    * `GAME_OVER`: Si el jugador pierde todos sus puntos de salud (`hp <= 0`) ([Game.cpp:440-442](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L440-L442)).
    * `PAUSED`: Al pulsar `P` o `ESCAPE` durante el combate ([Game.cpp:150-153](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L150-L153)).
* **`PAUSED`:**
  * Almacena el estado previo en `prevState` ([Game.cpp:77](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L77)).
  * Permite Reanudar (`state = prevState`), Reiniciar (`Reset()`) o Salir (`quit = true`) ([Game.cpp:86-103](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L86-L103)).
* **`GAME_OVER` y `VICTORY`:**
  * Al presionar `ENTER`, ejecutan `Reset()` para reiniciar el mundo a su estado original ([Game.cpp:162-167](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L162-L167)).

### 3.4. Separación entre actualización y dibujo
* La función [Game::Update](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L138-L169) actualiza exclusivamente la lógica, llamadas a IA, cálculo de colisiones y cronómetros.
* La función [Game::Draw](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L542-L551) y [Game::DrawScene](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L504-L540) ejecutan únicamente comandos gráficos de Raylib. Cuando el juego está en `PAUSED`, dibuja el cuadro de la escena previa (`prevState`) congelada y sobrepone el menú de pausa semitransparente.

### 3.5. Variables globales y dependencias entre módulos
* No existen variables globales mutables en el proyecto.
* Existen tablas constantes y paletas estáticas con visibilidad de unidad de traducción (`static`):
  * `table` en [Player.cpp:7](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L7): datos del bestiario.
  * `ART[6][12]` en [PixelArt.cpp:6](file:///C:/Users/Xavier/source/repos/Project1/Project1/PixelArt.cpp#L6): matrices de arte ASCII para sprites.
  * `LABELS[4]`, `CREAM`, `INK` en [CombatUI.cpp:6-8](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L6-L8).
  * `PAUSE_LABELS[3]` en [MapView.cpp:6](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L6).

### 3.6. Evaluación del patrón arquitectónico real
* **No es MVC puro:** A pesar de que los filtros de Visual Studio se denominan `Modelo`, `Vistas` y `Controladores` ([Project1.vcxproj.filters:16-24](file:///C:/Users/Xavier/source/repos/Project1/Project1/Project1.vcxproj.filters#L16-L24)), los componentes no respetan la separación estricta de responsabilidades de MVC:
  * El "Modelo" ([Player.cpp:45-54](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L45-L54)) incluye directamente `raylib.h` y lee entradas de hardware (`IsKeyDown`).
  * Las "Vistas" ([CombatUI.cpp:121-125](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L121-L125) y [MapView.cpp:390-398](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L390-L398)) leen eventos de mouse (`IsMouseButtonPressed`, `GetMousePosition`).
* **Conclusión arquitectónica:** Es una arquitectura de **Programación Orientada a Objetos (POO)** clásica con un orquestador central monolítico (`Game`). La separación entre lógica y renderizado es modular pero está pragmáticamente acoplada a las estructuras y funciones de Raylib.

---

## 4. FUNCIONES IMPLEMENTADAS

### 4.1. Movimiento del jugador
* **Responsable:** Clase `Player` ([Player.h:57-58](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L57-L58), [Player.cpp:45-68](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L45-L68)).
* **Implementación:**
  * Teclas leídas: `W`, `S`, `A`, `D` o flechas direccionales (`KEY_UP`, `KEY_DOWN`, `KEY_LEFT`, `KEY_RIGHT`).
  * Normaliza el vector de entrada diagonal dividiendo por `len = sqrt(dx*dx + dy*dy)`.
  * Multiplica por `speed = 180.0f` y `dt`.
  * Determina la orientación del personaje (`Facing::LEFT`, `Facing::RIGHT`, `Facing::UP`, `Facing::DOWN`) según el eje predominante.
  * Incrementa `animTimer += dt` si está en movimiento.

### 4.2. Mapas, cámara y colisiones
* **Responsable:** Clase `TileMap` ([TileMap.h:8-31](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.h#L8-L31), [TileMap.cpp:4-74](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L4-L74)).
* **Cámara:** **No existe ninguna cámara ni objeto `Camera2D` de Raylib.** El juego se renderiza en una ventana de tamaño fijo de 960x540 píxeles. El mapa completo (30 columnas x 16 filas x 32 píxeles = 960x512 píxeles) cabe estáticamente en la pantalla, reservando los 28 píxeles inferiores (512 a 540) para la barra de estado (*HUD*).
* **Colisiones:**
  * Método [TileMap::IsBlocked](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L36-L45): convierte las coordenadas del rectángulo del jugador o enemigo a coordenadas de celda mediante `std::floor` y comprueba si intersecta con celdas sólidas.
  * Celdas sólidas ([TileMap.cpp:31-34](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L31-L34)): `'#'` (árboles/muros), `'W'` (agua), `'H'` (casas), `'1'` y `'2'` (compuertas lógicas).
  * Deslizamiento en paredes ([TileMap.cpp:47-54](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L47-L54)): `TileMap::Move` prueba el movimiento primero en el eje X y luego en el eje Y por separado, permitiendo deslizarse a lo largo de muros.
* **Compuertas:**
  * Método [TileMap::OpenGate(int index)](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L56-L61): reemplaza las celdas marcadas con el caracter `'1' + index` por `'p'` (camino transitable).

### 4.3. Enemigos e inteligencia artificial
* **Responsable:** Clase `Enemy` ([Player.h:70-95](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L70-L95), [Player.cpp:87-134](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L87-L134)).
* **Máquina de estados de IA (`AIState`):**
  * `PATROL`: El enemigo elige de forma aleatoria una dirección cardinal o reposo (`patrolDir`) mediante un temporizador aleatorio de 1.0 a 2.5 segundos ([Player.cpp:121-124](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L121-L124)). Posee un radio de anclaje a su posición original (`home`): si se aleja más de 96.0 píxeles, fuerza su dirección de retorno hacia `home`. Velocidad: 40.0 px/s.
  * `CHASE`: Se activa si la distancia euclidiana hacia el jugador es menor a `detectRadius` (130 px para enemigos normales, 160 px para el boss). Persigue directamente al jugador a 75 px/s (85 px/s para el boss). Si la distancia supera `loseRadius` (220 px), regresa al estado `PATROL` ([Player.cpp:107-108](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L107-L108)).
* **Colisiones de enemigos:** Los enemigos también llaman a `map.Move(body, ...)` ([Player.cpp:133](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L133)), por lo que respetan los obstáculos sólidos del mapa.

### 4.4. Combate y acciones disponibles
* **Responsable:** Clases `Game` ([Game.cpp:208-498](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L208-L498)) y `CombatUI` ([CombatUI.cpp:10-125](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L10-L125)).
* **Mecánica:** Combate por turnos estricto en arena separada.
* **Acciones del jugador (`PlayerAction`):**
  1. `ATTACK`: Ejecuta `enemy.TakeDamage(player.CalculateDamage(enemy))` ([Game.cpp:303-309](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L303-L309)).
  2. `SKILL`: Requiere 10 MP (`SKILL_COST`). Aplica multiplicador de daño x1.8 (`SKILL_MULT`). Si el maná es insuficiente, emite mensaje al log y **no consume el turno** ([Game.cpp:311-336](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L311-L336)).
  3. `POTION`: Consume 1 poción y cura 40 HP (`POTION_HEAL`). Si no quedan pociones, cancela la acción sin consumir el turno ([Game.cpp:338-352](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L338-L352)).
  4. `FLEE`: Probabilidad fija del 50% (`GetRandomValue(1, 100) <= 50`). Si tiene éxito, empuja al jugador 64 píxeles lejos del enemigo, impone 2.0 segundos de inmunidad de encuentros y regresa al mapa ([Game.cpp:354-367, 466-489](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L354-L367)). Si falla, consume el turno y el enemigo ataca.
* **Fórmula de daño:**
  $$\text{daño} = \max\left(1, \; \lfloor\text{atk} \times \text{multiplicador}\rfloor - \text{def}_{\text{objetivo}} + \text{RNG}(-2, +3)\right)$$
  (Definida en [Player.cpp:33-37](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L33-L37)).

### 4.5. Vida, experiencia, niveles, inventario y objetos
* **Vida y Maná:** Gestionados en `Combatant` ([Player.h:27-28](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L27-L28)).
* **Experiencia:** **NO existe ningún sistema ni variable de puntos de experiencia (EXP).**
* **Niveles:** Subida de nivel automática al vencer a cualquier enemigo mediante [Player::LevelUp()](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L83): incrementa `level` en 1, `maxHp += 15`, `maxMp += 5`, `atk += 3`, `def += 2`. Seguidamente llama a `Player::FullRestore()` ([Player.cpp:84](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L84)), restaurando la vida y maná al máximo.
* **Inventario y objetos:** **NO existe un sistema de inventario general ni clases de objetos.** Solo existe una variable entera `int potions = 3` dentro de la clase `Player` ([Player.h:45](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L45)). No hay equipamiento, armas, armaduras ni consumibles adicionales.

### 4.6. Diálogos y NPCs
* **NO existen sistemas de diálogos ni NPCs implementados.** Los bloques marcados con `'H'` en el mapa son rendered como casitas decorativas sólidas con tejado y puerta mediante primitivas geométricas ([MapView.cpp:58-72](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L58-L72)), pero no contienen interacción.

### 4.7. Menús, pausa, reinicio, victoria y derrota
* **Menú de pausa:** [Game::UpdatePause](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L105-L132) y [MapView::DrawPause](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L400-L447). Posee tres opciones: "Reanudar", "Reiniciar" y "Salir". Se puede controlar mediante teclas direccionales + `ENTER` o haciendo clic con el mouse.
* **Reinicio:** [Game::Reset](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L56-L58) reinicializa completamente `TileMap`, `Player` y reconstruye la lista de enemigos extrayéndolos del mapa base.
* **Victoria:** [Game::DrawEndScreen("VICTORIA!", GOLD)](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L533). Se activa únicamente al matar al Dragón (`isBoss == true`). Al presionar `ENTER`, reinicia el juego.
* **Derrota:** [Game::DrawEndScreen("GAME OVER", RED)](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L529). Se activa cuando `player.hp <= 0`. Al presionar `ENTER`, reinicia el juego.

### 4.8. Guardado y carga
* **NO existe ningún sistema de guardado o carga de partida** (no se lee ni escribe ningún archivo de persistencia). Al cerrar la ventana o reiniciar, el progreso se pierde por completo.

### 4.9. Gráficos, animaciones y audio
* **Gráficos:**
  * Terreno dibujado con primitivas de Raylib: rectángulos para pasto, caminos y agua; círculos superpuestos para el follaje de los árboles; triángulos y rectángulos para las casas ([MapView.cpp:36-87](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L36-L87)).
  * Monstruos y jugador dibujados mediante rasterización de matrices 12x12 por software ([PixelArt.cpp:101-115](file:///C:/Users/Xavier/source/repos/Project1/Project1/PixelArt.cpp#L101-L115)).
  * Minimapa ([MapView.cpp:304-379](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L304-L379)): overlay superior derecho escalado a 4 píxeles por tile con representación de terreno, enemigos (puntos rojos) y jugador (cuadrado blanco). Se alterna con la tecla `M`.
* **Animaciones:**
  * En combate: oscilación vertical continua en los sprites mediante `sin(GetTime() * 3.0f) * 3.0f` ([CombatUI.cpp:67](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L67)).
  * En mapa (dibujo procedural): balanceo de piernas y cuerpo con `sin(animTimer * 16.0f)` ([MapView.cpp:139-164](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L139-L164)).
  * En mapa (con textura): cicla 4 fotogramas usando `(int)(animTimer * 8.0f) % 4` ([MapView.cpp:109](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L109)).
* **Audio:**
  * **NO existe audio en el proyecto.** No hay archivos de sonido ni música, no se llama a `InitAudioDevice()` de Raylib ni se utilizan funciones de sonido.

---

## 5. RECURSOS

### 5.1. Imágenes, sprites, fuentes, sonidos y mapas
* **Imágenes / Texturas:**
  * La única referencia a un archivo externo en todo el proyecto es `"assets/hero.png"` en [MapView.cpp:15-18](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L15-L18).
* **Fuentes:** No se cargan fuentes externas (`Font`). Se utiliza la fuente rasterizada interna por defecto de Raylib a través de `DrawText` y `MeasureText`.
* **Sonidos:** Ninguno.
* **Mapas:** No hay archivos `.tmx` ni `.json`. El mapa está codificado en texto dentro de [TileMap.cpp:6-23](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L6-L23).

### 5.2. Rutas de carga y directorio de trabajo esperado
* La ruta de carga esperada es `"assets/hero.png"` de forma relativa.
* El directorio de trabajo esperado para que cargue la textura es aquel desde donde `assets/hero.png` sea accesible.

### 5.3. Recursos referenciados que no estén presentes
* El archivo `"assets/hero.png"` y el directorio `assets/` **NO existen físicamente en el repositorio**.
* **Comportamiento ante su ausencia:** La función [MapView::Load()](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L12-L19) comprueba `if (FileExists("assets/hero.png"))`. Al no encontrarlo, asigna `hasSheet = false` y [MapView::DrawPlayer](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L137-L209) conmuta limpiamente a su rutina de dibujo procedimental mediante primitivas de Raylib sin arrojar error ni provocar fallos de ejecución.

---

## 6. ESTADO Y PROBLEMAS

### 6.1. Funciones incompletas y comentarios TODO/FIXME
* No existen comentarios con las etiquetas `TODO`, `FIXME`, `BUG` ni `HACK` en ningún archivo de la solución.
* Las mecánicas presentes están terminadas dentro de su alcance, pero carece deliberadamente de subsistemas complejos (sin inventario general, sin diálogos, sin guardado, sin audio).

### 6.2. Problemas confirmados y sospechas

#### Problemas Confirmados:
1. **Advertencia de compilación C4244 en [MapView.cpp:221](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L221):**
   ```cpp
   DrawEllipse(static_cast<int>(centerX), static_cast<int>(enemy.body.y + enemy.body.height),
               static_cast<int>(spriteSize / 2.0f), 4, Fade(BLACK, 0.30f));
   ```
   El compilador MSVC emite `warning C4244: 'argumento': conversión de 'int' a 'float'; posible pérdida de datos` debido a que en la firma de Raylib `DrawEllipse(int, int, float, float, Color)` el tercer argumento es convertido de `int` a `float`.
2. **Conflicto de enlazado LNK4098:**
   `LINK : warning LNK4098: la biblioteca predeterminada 'MSVCRT' entra en conflicto con otras bibliotecas; use la biblioteca /NODEFAULTLIB:biblioteca`.
   Ocurre porque el paquete precompilado de Raylib NuGet enlaza contra el runtime de release (`MSVCRT.lib`), mientras que la configuración `Debug` del proyecto enlaza con el runtime de depuración (`MSVCRTD.lib` / `/MDd`).
3. **Peligro en `TileMap::At` ante desbordamiento de columnas:**
   En [TileMap.cpp:28](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L28), si una fila en `tiles` tuviera por error menos de 30 caracteres, `tiles[r][c]` produciría acceso a memoria fuera de rango. (Actualmente todas las filas tienen 30 caracteres).

#### Sospechas / Comportamientos Frágiles:
1. **Atrapamiento tras huir del combate (`FLEE`):**
   En [Game.cpp:470-486](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L470-L486), al huir se empuja al jugador 64 píxeles en dirección opuesta al enemigo llamando a `map.Move(player.body, ...)`. Si el jugador se encuentra contra una esquina o callejón sin salida donde ningún eje es transitable, el desplazamiento es nulo (`0`), dejando al jugador pegado al enemigo cuando expiren los 2 segundos de `mapCooldown`.
2. **Ausencia de validación de dimensiones en textura externa:**
   En [MapView.cpp:113-118](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L113-L118), si un usuario coloca un archivo `assets/hero.png` con dimensiones distintas a 128x128 píxeles (4x4 celdas de 32x32), el código asume ciegamente pasos de `32.0f` píxeles sin verificar `heroSheet.width` ni `heroSheet.height`.

### 6.3. Verificación de compilación y ejecución
* **Compilación:** **COMPILACIÓN VERIFICADA**.
  * **Herramienta:** MSBuild versión `18.11.0` (MSVC toolset `14.51.36231`).
  * **Configuración:** `Debug|x64`.
  * **Resultado:** **Compilación exitosa (0 errores, 2 advertencias: C4244 y LNK4098)**.
  * **Binario generado:** `C:\Users\Xavier\source\repos\Project1\Project1\x64\Debug\Project1.exe`.
* **Ejecución:** **EJECUCIÓN NO VERIFICADA EN MODO INTERACTIVO**.
  * El ejecutable fue generado satisfactoriamente, pero no se ejecutó una sesión interactiva en vivo por tratarse de una ventana gráfica de escritorio que requiere renderizado OpenGL y entrada interactiva del usuario.

---

## 7. PUNTOS DE EXTENSIÓN

### 7.1. Nuevos enemigos o criaturas
1. Abrir [Player.cpp:7-16](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L7-L16) en la función `GetSpecies()` y agregar una nueva fila a la tabla `table`:
   ```cpp
   { 'G', "Orco", 6, 120, 26, 6, false, -1, 2, { 50, 120, 50, 255 }, { 100, 180, 100, 255 } },
   ```
2. Si el nuevo caracter no está entre `'A'` y `'F'`, actualizar el método [TileMap::ExtractSpawns](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L68) ampliando la condición:
   `if ((ch >= 'A' && ch <= 'F') || ch == 'G')`.
3. Colocar el caracter `'G'` en la matriz de texto del mapa en [TileMap.cpp:7-22](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L7-L22).

### 7.2. Nuevos mapas
1. Modificar `TileMap` ([TileMap.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.h)) para admitir un constructor que reciba `std::vector<std::string>` o cargue un archivo de texto.
2. Hacer dinámicas o parametrizables las constantes estáticas `COLS` y `ROWS` (actualmente fijas en 30 y 16 en [TileMap.h:11-12](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.h#L11-L12)).
3. En `Game` ([Game.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.h)), implementar una función de cambio de mapa que invoque `map.ExtractSpawns()` y reposicione al jugador.

### 7.3. Nuevas acciones de combate
1. Añadir el valor al enum `PlayerAction` en [GameState.h:9](file:///C:/Users/Xavier/source/repos/Project1/Project1/GameState.h#L9) (por ejemplo `DEFEND = 4`).
2. Ampliar el tamaño del arreglo `buttons[4]` en [CombatUI.h:20](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.h#L20) y los textos `LABELS` en [CombatUI.cpp:6](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L6).
3. Añadir el caso correspondiente en el `switch (action)` dentro de [Game::ExecutePlayerAction](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L302-L371).

### 7.4. Objetos e inventario
1. Crear una estructura `Item` con `std::string name; int healAmount;` etc.
2. Reemplazar `int potions = 3;` en [Player.h:45](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h#L45) por un contenedor `std::vector<Item> inventory;`.
3. Crear un submenú en `CombatUI` que liste los elementos del inventario al seleccionar `POTION` o `OBJETOS`.

### 7.5. Nuevas pantallas o estados
1. Añadir el estado al enum `GameState` en [GameState.h:3](file:///C:/Users/Xavier/source/repos/Project1/Project1/GameState.h#L3) (por ejemplo `INVENTORY`, `TITLE_SCREEN`).
2. Agregar la rama en [Game::Update](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L139-L169) para procesar la entrada del nuevo estado.
3. Agregar la rama en [Game::DrawScene](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp#L504-L540) para llamar a la función de dibujo correspondiente.

---

## 8. PREPARACIÓN PARA FUSIONAR CON OTRO RPG

### 8.1. Subsistemas potencialmente reutilizables
* **Módulo `PixelArt` ([PixelArt.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/PixelArt.h)):** 100% reutilizable. Es un motor de renderizado de sprites de bajo costo que solo requiere la función `DrawRectangle` de Raylib.
* **Algoritmo de colisión de `TileMap::Move` ([TileMap.cpp:47-54](file:///C:/Users/Xavier/source/repos/Project1/Project1/TileMap.cpp#L47-L54)):** Lógica limpia de comprobación AABB desacoplada en ejes X e Y para evitar atascos en esquinas.
* **IA de supramundo (`Enemy::UpdateAI` en [Player.cpp:101-134](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L101-L134)):** Rutina autocontenida de patrulla con tethering a punto base y persecución por radio.
* **Fórmula de daño de combate ([Player.cpp:33-37](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp#L33-L37)):** Fácilmente portable a cualquier sistema de combate RPG por turnos.

### 8.2. Dependencias que habría que trasladar
* Biblioteca **Raylib 5.5.0** (o sus cabeceras y binarios correspondientes).
* Tipos de datos fundamentales de Raylib utilizados en todas las entidades: `Rectangle`, `Vector2`, `Color`.

### 8.3. Nombres genéricos propensos a conflicto
Los siguientes identificadores globales no tienen *namespace* y colisionarán con casi cualquier otro proyecto RPG:
* Clases: `Game`, `Player`, `Enemy`, `Combatant`, `TileMap`, `MapView`, `CombatUI`.
* Enumeraciones: `GameState`, `CombatPhase`, `CombatResult`, `PlayerAction`, `AIState`, `Facing`.
* Estructura: `MonsterSpecies`.
* **Solución mandatoria previa a la fusión:** Envolver todo este código dentro de un espacio de nombres específico (por ejemplo `namespace SourceRPG { ... }`).

### 8.4. Suposiciones rígidas de diseño
* **Resolución:** Asume exactamente 960 x 540 píxeles. Todo el diseño de botones de `CombatUI` ([CombatUI.cpp:13](file:///C:/Users/Xavier/source/repos/Project1/Project1/CombatUI.cpp#L13)) y HUD ([MapView.cpp:277](file:///C:/Users/Xavier/source/repos/Project1/Project1/MapView.cpp#L277)) está posicionado con constantes numéricas fijas (*hardcoded*).
* **Ausencia de cámara:** Asume que todo el supramundo es una sola pantalla fija de 30x16 tiles de 32x32.
* **Control de entrada:** Entrada directa a teclado y mouse sin capa de abstracción de remapeo de controles.

### 8.5. Partes que requieren adaptación obligatoria
* `Player` y `Enemy` no pueden trasladarse como modelos puros sin arrastrar `raylib.h` porque contienen rectángulos físicos y llamadas a funciones de entrada.
* La clase `Game` mezcla bucle de ventana, gestión de estados y resolución de combates; debe dividirse si el proyecto destino ya cuenta con su propio gestor de escenas (*SceneManager*) o bucle principal.

---

## 9. RESUMEN PARA OTRO ASISTENTE

* **Qué juego es:** Un prototipo funcional de RPG en 2D desarrollado en C++20 y Raylib 5.5.0 con vista cenital y combate por turnos clásico (estilo Pokémon / Dragon Quest primitivo).
* **Cómo está organizado:** Solución en Visual Studio 2022 con código en la subcarpeta `Project1`. Sigue un diseño orientado a objetos organizado conceptualmente en filtros de VS como Modelo (`Player`, `TileMap`), Vistas (`MapView`, `CombatUI`, `PixelArt`) y Controlador (`Game`), aunque acoplado pragmáticamente a Raylib.
* **Qué tiene implementado:**
  1. Movimiento en 4 direcciones con colisiones AABB y deslizamiento en paredes.
  2. Supramundo de 30x16 tiles con compuertas que se abren al derrotar a ciertos jefes.
  3. IA enemiga en mapa con estados de patrulla y persecución según distancia euclidiana.
  4. Transición fluida a combate por turnos con 4 comandos (Atacar, Habilidad de 10 MP, Poción curativa de 40 HP y Huir con 50% de probabilidad y knockback).
  5. 6 especies de monstruos con atributos diferenciados y arte en matrices de caracteres 12x12.
  6. Subida de nivel y regeneración completa automática tras vencer enemigos.
  7. Menú de pausa interactivo por teclado o mouse, minimapa en tiempo real y pantallas de Game Over y Victoria.
* **Qué falta verificar y qué no existe:**
  * Compilación verificada exitosamente en `Debug|x64` (0 errores, 2 advertencias menores C4244 y LNK4098).
  * Ejecución interactiva no mantenida en vivo.
  * **No implementado:** Audio, sistema de diálogos, NPCs parlantes, inventario libre de objetos, puntos de experiencia numéricos, guardado/carga y cámara de seguimiento.
* **Qué archivos necesita revisar primero el otro asistente:**
  1. [GameState.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/GameState.h): Para comprender el catálogo de estados y transiciones.
  2. [Game.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.h) / [Game.cpp](file:///C:/Users/Xavier/source/repos/Project1/Project1/Game.cpp): Para entender el bucle de eventos, el inicio de combate y la resolución de victoria/derrota.
  3. [Player.h](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.h) / [Player.cpp](file:///C:/Users/Xavier/source/repos/Project1/Project1/Player.cpp): Para examinar las clases `Combatant`, `Player`, `Enemy` y la tabla estática de criaturas antes de integrarlas al nuevo proyecto.
