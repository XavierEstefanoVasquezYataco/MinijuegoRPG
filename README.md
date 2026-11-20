# Mini Aventura RPG — Versión 2 (v2)

Proyecto académico desarrollado en **C++20** con la biblioteca multimedia **Raylib 5.5.0** para el curso de *Programación de Videojuegos con C++*.

---

## 1. Visión General del Proyecto

**Mini Aventura RPG v2** evoluciona el prototipo inicial de exploración 2D y combate por turnos hacia una experiencia estructurada, pedagógicamente sólida y desacoplada mediante el patrón de diseño **MVC (Modelo-Vista-Controlador)** práctico. 

El proyecto incorpora selección de clases con identidad táctica, un mundo abierto expandido con cámara de seguimiento suave y minimapa, personajes no jugadores (NPCs) con diálogos progresivos, un sistema de experiencia y progresión persistente, y un combate reactivo con animaciones procedurales y efectos visuales dinámicos.

---

## 2. Novedades y Características de la Versión 2

### 2.1. Arquitectura MVC Práctica y Desacoplamiento de Hardware

El proyecto implementa una versión pragmática del patrón **Modelo-Vista-Controlador (MVC)** orientada al desarrollo de videojuegos con ciclo de vida en tiempo real:

* **Modelo (`Project1/Modelo/`):**
  * Contiene los datos puros y las reglas de negocio del juego: estadísticas de combate, progresión de nivel, fórmulas de daño, estado del mapa de casillas (60x32), colisiones y narrativa progresiva de los NPCs.
  * Archivos: [`GameState.h`](Project1/Modelo/GameState.h), [`Player.h`](Project1/Modelo/Player.h) / [`Player.cpp`](Project1/Modelo/Player.cpp), [`TileMap.h`](Project1/Modelo/TileMap.h) / [`TileMap.cpp`](Project1/Modelo/TileMap.cpp), [`NPC.h`](Project1/Modelo/NPC.h) / [`NPC.cpp`](Project1/Modelo/NPC.cpp).
  * Regla de diseño: Cero llamadas a primitivas gráficas de Raylib y cero consultas directas de hardware (`IsKeyDown`, `GetMousePosition`).
* **Vistas (`Project1/Vistas/`):**
  * Clases pasivas encargadas de la representación visual en pantalla (framebuffer) en resolución virtual 960x540. Reciben referencias constantes (`const &`) de las entidades del modelo y no modifican el estado de la partida.
  * Archivos: [`TitleView.h`](Project1/Vistas/TitleView.h) / [`TitleView.cpp`](Project1/Vistas/TitleView.cpp), [`MapView.h`](Project1/Vistas/MapView.h) / [`MapView.cpp`](Project1/Vistas/MapView.cpp), [`CombatUI.h`](Project1/Vistas/CombatUI.h) / [`CombatUI.cpp`](Project1/Vistas/CombatUI.cpp), [`DialogueView.h`](Project1/Vistas/DialogueView.h) / [`DialogueView.cpp`](Project1/Vistas/DialogueView.cpp), [`PixelArt.h`](Project1/Vistas/PixelArt.h) / [`PixelArt.cpp`](Project1/Vistas/PixelArt.cpp).
  * Módulo especializado de animación: [`CombatPresentation.h`](Project1/Vistas/CombatPresentation.h) / [`CombatPresentation.cpp`](Project1/Vistas/CombatPresentation.cpp) gestiona interpolación de barras (`160 pts/s`), embestidas visuales (*lunge*), destellos de daño (*hit flash*), sacudida de pantalla (*screen shake*), textos flotantes y congelamiento del tiempo durante la pausa.
  * Exponen consultas geométricas puras (`GetHoveredButton`, `GetChoiceButtonRect`) para que el controlador conozca la interacción del usuario sin ejecutar lógica dentro del renderizado.
* **Controlador (`Project1/Controladores/`):**
  * Orquestador central de la aplicación ([`Game.h`](Project1/Controladores/Game.h) / [`Game.cpp`](Project1/Controladores/Game.cpp)). Captura la entrada física de hardware (teclado y ratón de Raylib), ejecuta las transiciones de la máquina de estados (`GameState`), actualiza el tiempo (`dt`), comanda el movimiento y las colisiones, y delega el dibujo a las vistas pasivas.
* **Punto de Entrada (`Project1/main.cpp`):**
  * Inicializa el ciclo de vida instanciando el objeto `Game` y llamando a `Game::Run()`, manteniendo el arranque limpio y modular.

#### Relación entre Clases Principales (Flujo Real de Ejecución)
1. `main()` crea `Game` e inicia `Run()`.
2. `Game` inicializa `TileMap`, `Player`, la lista de `Enemy`, los `NPC` y las vistas pasivas (`MapView`, `CombatUI`, `TitleView`, `DialogueView`, `CombatPresentation`).
3. En cada fotograma, `Game::Update(dt)` lee el hardware de Raylib:
   * En exploración (`MAP`), calcula el vector direccional y comanda a `Player::Update()` y `map.Move()`.
   * Si hay contacto con un enemigo, transiciona a `COMBAT` e invoca `combatPres.OnCombatStart()`.
   * En combate (`COMBAT`), procesa las elecciones 2x2, calcula daño en `Combatant::CalculateDamage()` y activa disparadores visuales en `CombatPresentation`.
4. En cada fotograma, `Game::Draw()` invoca el método correspondiente de la vista (`mapView.DrawWorld()`, `ui.Draw()`, etc.) pasándole datos en solo lectura.

#### Diferencia entre Filtros de Visual Studio y Carpetas Físicas
* **Filtros de Visual Studio (`.vcxproj.filters`):** Son etiquetas puramente lógicas en formato XML utilizadas por el IDE de Visual Studio para agrupar archivos en el *Explorador de Soluciones*. No alteran la ubicación física en el disco duro ni la visualización de directorios en GitHub.
* **Carpetas Físicas del Repositorio:** Son directorios reales en el sistema de archivos (`Project1/Modelo`, `Project1/Vistas`, `Project1/Controladores`). Esta organización física permite que en GitHub el código fuente se explore de forma ordenada según su responsabilidad arquitectónica y que las rutas relativas en los `#include` sean consistentes entre diferentes plataformas y compiladores.

#### Limitaciones Arquitectónicas Identificadas
* **Acoplamiento de recursos visuales en el Modelo:** `Player.cpp` y `NPC.cpp` incluyen `PixelArt.h` para acceder a identificadores numéricos de sprites (`ART_WARRIOR`, `ART_ELDER`, etc.). En una arquitectura MVC pura y estricta, el modelo solo debería almacenar identificadores abstractos y la vista mapearlos a gráficos.
* **Controlador Monolítico:** La clase `Game` concentra la orquestación de todas las escenas (Título, Mapa, Combate, Diálogos y Pausa). Un paso evolutivo posterior sería implementar un gestor de estados modular (*State Pattern*) donde cada escena sea un controlador independiente.
* **Dependencia de tipos de Raylib en el Modelo:** `Player.h`, `TileMap.h` y `NPC.h` utilizan `Rectangle` y `Vector2` de Raylib para colisiones por simplicidad. Aunque no realizan renderizado, vinculan los tipos matemáticos a la biblioteca gráfica externa.

### 2.2. Pantalla de Título y Selección de Héroe
* **Menú Principal:** Opciones para *Nueva Partida*, *Continuar* (deshabilitada con aviso informativo por estar en fase de desarrollo), *Controles* y *Salir* con confirmación (`[S]/[N]`).
* **Tres Arquetipos Diferenciados:**
  1. **Guerrero:** Alta vitalidad y armadura (120 HP, 6 DEF, 15 MP). Habilidad: *"Golpe Fuerte"* (daño x2.0, coste 10 MP). Tasa de huida: 50%.
  2. **Maga:** Gran reserva mística y daño arcano (80 HP, 2 DEF, 40 MP). Habilidad: *"Centella Arcana"* (daño x2.6, coste 12 MP). Tasa de huida: 50%.
  3. **Exploradora:** Agilidad balanceada (100 HP, 4 DEF, 25 MP). Mayor velocidad de movimiento en el mapa (+15%) y ventaja táctica de huida (65%). Habilidad: *"Tiro Crítico"* (daño x2.2, coste 10 MP).

### 2.3. Gran Mapa (60x32) y Cámara Dinámica 2D
* **Cuadrícula de 1920x1024 píxeles:** Dividida en cuatro sectores orgánicos conectados:
  1. **Aldea Segura (columnas 0–16):** Casas, caminos adoquinados, sin monstruos hostiles, curandera y NPCs tutoriales.
  2. **Pradera Abierta (columnas 17–28):** Slimes salvajes y Murciélago guardián de la Compuerta 1.
  3. **Bosque Sombrío (columnas 29–39):** Goblins del bosque y Lobo guardián de la Compuerta 2.
  4. **Ruinas Antiguas (columnas 40–59):** Esqueletos guerreros y cubil del Dragón (jefe final).
* **Cámara (`Camera2D`):** Centrada suavemente en el héroe con restricción de márgenes (*clamping*) para evitar mostrar el vacío exterior.
* **Minimapa Contextual:** Escala proporcional de 180x96 píxeles en la esquina superior derecha con visor del encuadre actual (*frustum*), alternable con la tecla `M`.

### 2.4. NPCs, Diálogos y Curandera de la Aldea
* **Mentor Eldrin:** Guía tutorial en la aldea; su diálogo progresa dinámicamente conforme se derrotan guardianes y abren compuertas.
* **Curandera Elena:** Ofrece restauración completa de salud y maná con confirmación interactiva `[S] SÍ` / `[N] NO`. Aplica regla anti-acumulación de pociones: si el héroe posee menos de 2 pociones, las repone a exactamente 2; si posee 2 o más, no altera el inventario.
* **Guardia Rurik y Explorador Varek:** NPCs situados cerca de zonas de transición que advierten sobre los guardianes y el peligro inminente del Dragón.
* **Carteles Informativos:** Distribuidos en cruces de caminos con indicaciones de orientación y advertencias de peligro.

### 2.5. Experiencia, Niveles y Persistencia Estricta
* **Experiencia Acumulativa:** Cada especie de enemigo derrotada otorga una recompensa fija de EXP (Slime: 25, Murciélago: 50, Goblin: 60, Lobo: 90, Esqueleto: 110, Dragón: 300).
* **Fórmula de Nivel:** $\text{expNext} = \text{nivel} \times 50$. Admite subidas de múltiples niveles continuos tras una sola recompensa mayor.
* **Persistencia entre Batallas:** Al vencer o huir, el héroe conserva exactamente el HP y MP resultantes de la pelea en memoria durante toda la sesión.
* **Subida de Nivel sin Curación Mágica:** Incrementar de nivel aumenta los topes máximos (`maxHp`, `maxMp`, `atk`, `def`) según la clase, pero preserva los valores actuales de HP y MP.
* **Poción Inteligente:** Con salud al máximo o sin existencias no consume turno ni inventario. Con daño, recupera hasta el tope y reporta en pantalla el HP efectivamente curado.

### 2.6. Pulido Visual y Sensación de Combate (*Game Feel*)
* **Escenario Adaptativo por Bioma:** Fondo procedural según el sector donde comenzó la batalla (Pradera celeste, Bosque crepuscular o Ruinas cenizas).
* **Barras Animadas:** Interpolación suave hacia los valores numéricos reales sin bloquear la lógica de juego.
* **Embestida (*Lunge*):** Desplazamiento visual del atacante hacia su objetivo y retorno fluido a su posición original.
* **Sensación de Impacto:** Destello blanco/rojo del defensor (*hit flash*), sacudida leve del escenario (*screen shake*) y números flotantes ascendentes con desvanecimiento alfa.
* **Congelamiento en Pausa:** El reloj interno de presentación se detiene completamente al pausar, eliminando saltos o desincronizaciones al reanudar.

---

## 3. Controles del Juego

| Contexto | Acción | Teclado | Ratón |
| :--- | :--- | :--- | :--- |
| **Exploración** | Desplazamiento del héroe | `W`, `A`, `S`, `D` / Flechas | — |
| **Exploración** | Interactuar con NPCs / Carteles | `E` | — |
| **Exploración** | Alternar Minimapa | `M` | — |
| **Exploración / Menús** | Pausa / Menú de opciones | `P` / `ESCAPE` | — |
| **Diálogos** | Avanzar texto | `E` / `ENTER` / `ESPACIO` | Clic en botón Continuar |
| **Diálogos (Elena)** | Confirmar / Cancelar Sanación | `S` (Sí) / `N` (No) / `ESC` | Clic en `[S] SÍ` / `[N] NO` |
| **Combate** | Navegar comandos 2x2 | `W`, `A`, `S`, `D` / Flechas | Apuntar sobre el botón |
| **Combate** | Confirmar acción | `ENTER` / `ESPACIO` | Clic izquierdo sobre el botón |
| **Menú Principal** | Navegación y confirmación | Flechas + `ENTER` / `ESPACIO` | Hover y clic izquierdo |

---

## 4. Estructura de Archivos del Proyecto

```
MiniAventuraRPG_v2/
├── Project1.slnx                     # Solución de Visual Studio 2022
├── README.md                         # Documentación general y manual de usuario
├── INFORME_V2.md                     # Informe técnico detallado de evolución v1 vs v2
├── CAMBIOS_DOCUMENTACION.md          # Guía para la actualización del informe Word universitario
├── PRUEBAS_MANUALES.md               # Matriz de casos de prueba funcional y manual
├── packages/                         # Dependencias NuGet nativas
│   └── raylib.5.5.0/                 # Raylib 5.5.0 (encabezados y binarios estáticos x64)
└── Project1/                         # Código fuente del proyecto (Organización Física MVC)
    ├── main.cpp                      # Punto de entrada de la aplicación
    ├── packages.config               # Configuración de dependencias NuGet
    ├── Project1.vcxproj              # Proyecto MSBuild C++20 con rutas actualizadas
    ├── Project1.vcxproj.filters      # Filtros lógicos en Visual Studio (Modelo, Vistas, Controladores)
    ├── Modelo/                       # Capa de Modelo (Lógica de Dominio y Datos Puros)
    │   ├── GameState.h               # Enumeraciones de estado global y fases de combate
    │   ├── Player.h / Player.cpp     # Clases Combatant, Player, Enemy y tablas de arquetipos
    │   ├── TileMap.h / TileMap.cpp   # Matriz de mapa 60x32, colisiones y compuertas
    │   └── NPC.h / NPC.cpp           # Modelo de personajes no jugadores y carteles
    ├── Vistas/                       # Capa de Vistas (Renderizado Pasivo y Presentación)
    │   ├── CombatUI.h / .cpp         # Vista pasiva de la arena de combate y registro de eventos
    │   ├── CombatPresentation.h/.cpp # Módulo de presentación: animaciones, barras y biomas
    │   ├── MapView.h / MapView.cpp   # Vista del supramundo, cámara 2D, HUD y minimapa
    │   ├── PixelArt.h / PixelArt.cpp # Rasterizador procedimental de sprites 12x12
    │   ├── TitleView.h / .cpp        # Vista de pantalla de título, controles y selección
    │   └── DialogueView.h / .cpp     # Vista de cuadros de diálogo y confirmación interactiva
    └── Controladores/                # Capa de Controladores (Orquestación del Bucle de Juego)
        └── Game.h / Game.cpp         # Controlador orquestador principal
```

---

## 5. Instrucciones de Compilación y Ejecución

### Requisitos del Sistema
* **Sistema Operativo:** Windows 10 / 11 (64-bit).
* **Entorno de Desarrollo:** Visual Studio 2022 (versión 17.0 o superior) con la carga de trabajo *Desarrollo para el escritorio con C++*.
* **Compilador:** MSVC con soporte para C++20 (`/std:c++20`).
* **SDK de Windows:** Windows 10 SDK (versión 10.0.26100.0 o compatible).
* **Biblioteca Multimedia:** Raylib 5.5.0 (integrada mediante paquete NuGet estático incluido en la carpeta `packages/`).

### Compilación y Ejecución desde Visual Studio 2022
1. Abrir la solución [`Project1.slnx`](Project1.slnx) en Visual Studio 2022.
2. Asegurarse de que el proyecto `Project1` esté configurado como *Proyecto de inicio*.
3. Seleccionar la configuración deseada en la barra superior:
   * **Configuración:** `Debug` o `Release`.
   * **Plataforma:** `x64`.
4. Compilar con el atajo `Ctrl + Mayús + B` (o menú *Compilar > Compilar solución*).
5. Iniciar la ejecución con `F5` (con depurador) o `Ctrl + F5` (sin depurador).

### Ejecución de los Binarios desde la Carpeta de Salida
Dado que la biblioteca Raylib 5.5.0 está vinculada estáticamente a través del archivo de destinos NuGet, los ejecutables son independientes y no requieren DLLs externas en su carpeta:
* **Ejecutable Debug:** `x64\Debug\Project1.exe` o `Project1\x64\Debug\Project1.exe`
* **Ejecutable Release:** `Project1\x64\Release\Project1.exe`

---

## 6. Historial de Commits por Etapas (`mejora-v2`)

1. `309eb1d`: **Etapa 1:** Separación de responsabilidades MVC, desacoplamiento de hardware y resolución de advertencia C4244.
2. `42be241`: **Etapa 2:** Pantalla de título, pantalla de controles y selección de los tres héroes con arquetipos balanceados.
3. `a86b3cd`: **Etapa 3:** Gran mapa de 60x32 tiles (1920x1024 px), cámara 2D con clamping y minimapa contextual.
4. `074e7ab`: **Etapa 4:** NPCs estáticos, diálogos progresivos, carteles informativos y curandera con confirmación interactiva `[S]/[N]`.
5. `4f16dac`: **Etapa 5:** Sistema de experiencia acumulativa (`level * 50`), subida de nivel escalonada y persistencia estricta de salud y maná.
6. `06ed2f8`: **Etapa 6:** Pulido visual de combate con módulo `CombatPresentation`, embestida de atacantes, barras interpoladas y biomas procedimentales.
7. *Pendiente / Alcance Futuro:* **Etapa 7:** Sistema de guardado y carga persistente en disco (`SaveSystem`).

---

## 7. Créditos y Contexto Académico

* **Curso:** Programación de Videojuegos con C++
* **Universidad:** Universidad Tecnológica del Perú (UTP)
* **Estudiante:** Xavier Estéfano Vásquez Yataco (U24237289)
* **Motor / Biblioteca:** Raylib 5.5.0 por Ramon Santamaria (@raysan5)
