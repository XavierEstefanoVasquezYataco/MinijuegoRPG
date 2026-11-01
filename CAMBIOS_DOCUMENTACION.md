# GUÍA DE ACTUALIZACIÓN DEL INFORME ACADÉMICO EN WORD (V2)

**Documento Destino:** Informe oficial del proyecto universitario (*Programación de Videojuegos con C++*)  
**Proyecto:** Mini Aventura RPG — Versión 2 (v2)  
**Objetivo de esta guía:** Proveer al estudiante el contenido exacto, las tablas formales de requisitos, los diagramas y las recomendaciones de capturas de pantalla para actualizar el documento de entrega final en Microsoft Word.

---

## 1. RESUMEN DE CAMBIOS ESTRUCTURALES

| Sección del Documento Word | Acción Requerida | Justificación Técnica v2 |
| :--- | :--- | :--- |
| **Portada y Metadatos** | Actualizar título a *Versión 2 (v2)* y fecha. | Reflejar la nueva versión del proyecto y la rama de trabajo. |
| **Introducción y Objetivos** | Incorporar los objetivos de modularización MVC y mejora de la experiencia de juego (*game feel*). | Justificar académicamente el rediseño sobre el prototipo inicial. |
| **Requisitos del Sistema** | Reemplazar listas informales por tablas estructuradas de RF y RNF. | Mayor rigor formal en la especificación de ingeniería de software. |
| **Arquitectura y Diseño** | Incluir el diagrama de flujo MVC y el desglose de responsabilidades de Modelo, Vista y Controlador. | Demostrar el desacoplamiento de la biblioteca Raylib respecto al modelo. |
| **Manual de Usuario** | Actualizar tablas de controles (teclado y ratón) y descripción de mecánicas. | Explicar nuevas pantallas: Selección de héroe, minimapa, diálogos interactivos y combate dinámico. |
| **Pruebas y Verificación** | Agregar la matriz de pruebas funcionales y la sección de limitaciones conocidas. | Evidenciar verificación rigurosa y honestidad sobre el alcance de guardado. |

---

## 2. ESPECIFICACIÓN FORMAL DE REQUISITOS (TABLAS PARA WORD)

### 2.1. Requisitos Funcionales (RF)

Copiar esta tabla directamente en la sección de Requisitos del documento Word:

| Código | Nombre del Requisito | Descripción | Prioridad | Estado en v2 |
| :---: | :--- | :--- | :---: | :---: |
| **RF-01** | Pantalla de Título y Menú | Menú principal interactivo con opciones: Nueva Partida, Continuar, Controles y Salir. | Alta | **Implementado** |
| **RF-02** | Selección de Héroe | Permitir al jugador elegir entre tres clases balanceadas (Guerrero, Maga, Exploradora) con estadísticas e identidades únicas. | Alta | **Implementado** |
| **RF-03** | Gran Mundo Expandido | Mapa continuo de 60 columnas por 32 filas (1920x1024 px) dividido en cuatro sectores (Aldea, Pradera, Bosque y Ruinas). | Alta | **Implementado** |
| **RF-04** | Cámara con Seguimiento 2D | Cámara suave que sigue al héroe en el mapa manteniendo la vista centrada y aplicando *clamping* en los bordes del mundo. | Alta | **Implementado** |
| **RF-05** | Minimapa Contextual | Visualización en tiempo real del mapa a escala reducida en la esquina superior derecha, con visor del encuadre actual (*frustum*) y alternable mediante tecla `M`. | Media | **Implementado** |
| **RF-06** | Compuertas y Progresión | Dos compuertas bloqueantes que dividen los sectores, las cuales se abren irreversiblemente al derrotar a sus guardianes asignados. | Alta | **Implementado** |
| **RF-07** | NPCs y Diálogos Progresivos | Personajes estáticos en la aldea y caminos (Mentor, Guardia, Explorador) que ofrecen pistas y actualizan sus diálogos según el progreso. | Media | **Implementado** |
| **RF-08** | Curandera de la Aldea | Personaje Elena que restaura por completo HP y MP mediante confirmación `[S] SÍ` / `[N] NO`, y repone pociones a exactamente 2 si se tienen menos. | Alta | **Implementado** |
| **RF-09** | Carteles Informativos | Al menos 5 carteles legibles en el supramundo con indicaciones de dirección y advertencias sobre zonas de peligro. | Baja | **Implementado** |
| **RF-10** | Combate por Turnos Dinámico | Sistema de combate táctico con 4 acciones: Atacar, Habilidad Especial, Poción Inteligente y Huida con tasa porcentual. | Alta | **Implementado** |
| **RF-11** | Efectos Visuales de Combate | Embestida (*lunge*), destello de daño (*hit flash*), sacudida de pantalla (*screen shake*), barras de salud/maná animadas con `dt` y biomas de fondo adaptativos. | Media | **Implementado** |
| **RF-12** | Sistema de Experiencia y Niveles | Curva matemática $\text{expNext} = \text{nivel} \times 50$, recompensas por especie y soporte para subidas de múltiples niveles continuos. | Alta | **Implementado** |
| **RF-13** | Persistencia de Recursos | Conservación estricta de HP y MP restantes entre batallas sucesivas dentro de la sesión de juego activa. | Alta | **Implementado** |
| **RF-14** | Pausa y Congelamiento | Pausar la simulación en cualquier momento congelando la lógica del mundo y los temporizadores de animaciones de combate. | Media | **Implementado** |
| **RF-15** | Guardado y Carga en Disco | Serialización de la partida en archivo físico para reanudar el progreso entre ejecuciones del juego. | Alta | *Pendiente (Etapa 7)* |

---

### 2.2. Requisitos No Funcionales (RNF)

| Código | Requisito No Funcional | Criterio de Cumplimiento Técnico |
| :---: | :--- | :--- |
| **RNF-01** | Rendimiento y Tasa de Cuadros | El videojuego debe ejecutarse de forma estable a **60 FPS fijos** en resolución nativa de 960x540 píxeles. |
| **RNF-02** | Arquitectura y Mantenibilidad | Separación desacoplada bajo el patrón **MVC práctico**: las clases de Modelo y Vista no deben leer hardware ni acoplarse entre sí. |
| **RNF-03** | Estándar de Lenguaje | Código fuente estrictamente apegado a la especificación estándar **C++20** (`stdcpp20`), haciendo uso de tipos semánticos y algoritmos modernos. |
| **RNF-04** | Portabilidad y Dependencias | Cero dependencias de rutas absolutas de la máquina de desarrollo; integración limpia de **Raylib 5.5.0** mediante paquete NuGet estático. |
| **RNF-05** | Calidad de Compilación | Cero advertencias del compilador en modo `/W3` tanto en configuraciones `Debug|x64` como `Release|x64`. |
| **RNF-06** | Usabilidad e Interfaz Gráfica | Compatibilidad dual para la navegación de menús y selección de comandos mediante **teclado y ratón**. |
| **RNF-07** | Consistencia Visual | Paleta visual unificada azul noche (`#121216`), bordes dorados cálidos y sprites rasterizados por código en cuadrícula proporcional. |
| **RNF-08** | Robustez y Tolerancia a Fallos | Prevención matemática de divisiones por cero en barras porcentuales y protección contra desbordamiento en arreglos de mapas y colisiones. |

---

## 3. TEXTOS SUGERIDOS PARA LA SECCIÓN DE ARQUITECTURA

Inserte los siguientes párrafos en el capítulo de *Diseño de Software / Arquitectura del Sistema*:

> *"En la Versión 2 de Mini Aventura RPG se implementó una arquitectura inspirada en el patrón Modelo-Vista-Controlador (MVC), adaptada pragmáticamente a las exigencias de un motor en tiempo real. 
> 
> La capa **Modelo** comprende las clases `Player`, `Enemy`, `TileMap` y `NPC`. Estas clases son completamente independientes de la biblioteca gráfica Raylib; almacenan únicamente estructuras de datos numéricos, estados booleanos y lógica de negocio pura (como la fórmula de experiencia o las colisiones rectangulares AABB).
> 
> La capa **Vista** está integrada por `TitleView`, `MapView`, `CombatUI`, `DialogueView`, `PixelArt` y el módulo auxiliar de presentación `CombatPresentation`. Su función es exclusivamente receptiva: reciben el estado del juego mediante referencias constantes (`const &`) y proyectan primitivas gráficas en la ventana. No modifican variables del jugador ni invocan llamadas de teclado o ratón.
> 
> Finalmente, la clase `Game` asume el rol de **Controlador central**. Captura los eventos físicos de hardware mediante las funciones de entrada de Raylib (`IsKeyPressed`, `GetMousePosition`), consulta a las vistas las regiones geométricas interactivas y despacha las modificaciones correspondientes hacia el modelo, garantizando una trazabilidad limpia y facilitando futuras pruebas unitarias."*

---

## 4. LISTA DE CAPTURAS DE PANTALLA RECOMENDADAS PARA EL INFORME

Para ilustrar de forma visual las mejoras en el informe Word, se recomienda capturar las siguientes escenas durante la ejecución de `x64\Debug\Project1.exe`:

1. **Pantalla de Título:** Mostrar el menú principal centrado con el botón *"Continuar"* deshabilitado indicando su estado, y los comandos de navegación en el pie de página.
2. **Selección de Héroe:** Capturar las tres tarjetas de arquetipos (Guerrero, Maga, Exploradora) destacando los valores numéricos y la habilidad especial de cada clase.
3. **Exploración en la Aldea:** Encuadre centrado en el héroe dentro de la aldea segura, mostrando las casas, el camino de adoquines, los NPCs y el minimapa en la esquina superior derecha.
4. **Diálogo con la Curandera Elena:** Mostrar el cuadro de diálogo inferior con las opciones interactivas `[S] SÍ` / `[N] NO`.
5. **Combate con Animación y Bioma:** Capturar una batalla activa en el sector del Bosque o las Ruinas mientras se ejecuta una embestida (*lunge*) con el efecto de destello (*hit flash*) y los números flotantes de daño.
6. **Pantalla de Pausa:** Demostrar la superposición del menú de pausa oscurecido durante la exploración o el combate.
7. **Pantalla de Victoria o Game Over:** Mostrar el desenlace tras derrotar al Dragón en las ruinas o tras caer en combate.

---

## 5. SECCIÓN DE CONCLUSIONES SUGERIDA PARA WORD

> *"La evolución de Mini Aventura RPG hacia su versión 2 demostró que la aplicación rigurosa de patrones de diseño como MVC permite transformar un prototipo rudimentario en un videojuego estructurado, extensible y pedagógicamente sobresaliente. La experiencia del jugador se incrementó notablemente mediante la diferenciación de clases, la ampliación del mundo a 60x32 tiles con cámara de seguimiento y la incorporación de efectos visuales reactivos en combate. Asimismo, la persistencia en memoria y la progresión matemática de niveles introdujeron una capa estratégica sólida que valoriza cada enfrentamiento. Como alcance futuro inmediato, se proyecta la implementación del módulo de serialización física en disco para consolidar la persistencia de partidas."*
