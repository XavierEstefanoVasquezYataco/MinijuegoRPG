# Mini Aventura RPG — Versión 2 (v2)

Proyecto académico desarrollado en **C++20** con la biblioteca multimedia **Raylib 5.5.0** para el curso de *Programación de Videojuegos con C++*.

---

## 1. Visión General del Proyecto

**Mini Aventura RPG v2** evoluciona el prototipo inicial de exploración 2D y combate por turnos hacia una experiencia estructurada, pedagógicamente sólida y desacoplada mediante el patrón de diseño **MVC (Modelo-Vista-Controlador)** práctico. 

El proyecto incorpora selección de clases con identidad táctica, un mundo abierto expandido con cámara de seguimiento suave y minimapa, personajes no jugadores (NPCs) con diálogos progresivos, un sistema de experiencia y progresión persistente, y un combate reactivo con animaciones procedurales y efectos visuales dinámicos.

---

## 2. Novedades y Características de la Versión 2

### 2.1. Arquitectura MVC Práctica y Desacoplamiento de Hardware
* **Modelo (`Model`):** Clases de datos y lógica de negocio puras ([`Player`](Project1/Player.h), [`Enemy`](Project1/Player.h), [`TileMap`](Project1/TileMap.h), [`NPC`](Project1/NPC.h)). Cero llamadas a primitivas gráficas de Raylib y cero consultas directas de hardware.
* **Vistas (`Views`):** Clases pasivas de renderizado ([`TitleView`](Project1/TitleView.h), [`MapView`](Project1/MapView.h), [`CombatUI`](Project1/CombatUI.h), [`DialogueView`](Project1/DialogueView.h), [`PixelArt`](Project1/PixelArt.h)). Reciben referencias constantes (`const &`) del estado y exponen consultas geométricas puras (`GetHoveredButton`, `GetChoiceButtonRect`).
* **Controlador (`Controller`):** Orquestador centralizado ([`Game`](Project1/Game.h)). Captura la entrada física de hardware, gestiona la máquina de estados (`GameState`) y coordina las transiciones.
* **Presentación de Combate (`CombatPresentation`):** Módulo dedicado ([`CombatPresentation`](Project1/CombatPresentation.h)) que gestiona interpolación de barras de salud y maná, embestidas visuales (*lunge*), destellos de daño (*hit flash*), sacudida de pantalla (*screen shake*), textos flotantes y congelamiento del tiempo durante la pausa.

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
└── Project1/                         # Código fuente del proyecto
    ├── main.cpp                      # Punto de entrada de la aplicación
    ├── GameState.h                   # Enumeraciones de estado global y fases de combate
    ├── Game.h / Game.cpp             # Controlador orquestador principal
    ├── Player.h / Player.cpp         # Clases Combatant, Player, Enemy y tablas de arquetipos
    ├── TileMap.h / TileMap.cpp       # Matriz de mapa 60x32, colisiones y compuertas
    ├── MapView.h / MapView.cpp       # Vista del supramundo, cámara 2D, HUD y minimapa
    ├── CombatUI.h / CombatUI.cpp     # Vista pasiva de la arena de combate y registro de eventos
    ├── CombatPresentation.h / .cpp   # Módulo de presentación: animaciones, barras y biomas
    ├── TitleView.h / TitleView.cpp   # Vista de la pantalla de título, controles y selección
    ├── DialogueView.h / .cpp         # Vista de cuadros de diálogo y confirmación interactiva
    ├── NPC.h / NPC.cpp               # Modelo puro de personajes no jugadores y carteles
    ├── PixelArt.h / PixelArt.cpp     # Rasterizador procedimental de sprites 12x12
    ├── Project1.vcxproj              # Proyecto MSBuild C++20
    └── Project1.vcxproj.filters      # Filtros lógicos en Visual Studio (Modelo, Vistas, Controladores)
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
