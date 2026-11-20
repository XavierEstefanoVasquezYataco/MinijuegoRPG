/**
 * @file TileMap.h
 * @brief Definición del modelo del mapa de casillas (TileMap) y detección de colisiones.
 *
 * Capa de Arquitectura: Modelo (Model) - Lógica de Escenario y Geometría.
 *
 * Propósito y Responsabilidades:
 * - Almacenar la estructura matricial de 60 columnas x 32 filas que define el supramundo.
 * - Clasificar casillas sólidas y transitables (árboles, agua, muros, compuertas, caminos).
 * - Proveer detección y resolución de colisiones por cajas delimitadoras alineadas con los ejes (AABB).
 * - Gestionar el estado de las compuertas que bloquean el avance entre biomas.
 * - Extraer los puntos de aparición de monstruos (`Spawn`) al inicializar el mundo.
 *
 * Relaciones Principales:
 * - Consultada por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) para mover entidades y desbloquear compuertas.
 * - Consultada por las Vistas ([Vistas/MapView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/MapView.h)) para el renderizado del supramundo y el minimapa.
 * - Utilizada por `Player` y `Enemy` para restringir el desplazamiento físico sobre el terreno.
 *
 * Observaciones Arquitectónicas:
 * - Aunque incluye tipos geométricos de Raylib (`Rectangle`, `Vector2`), no realiza ninguna
 *   llamada a funciones de renderizado ni consulta hardware de entrada, manteniendo el modelo desacoplado.
 */

#pragma once
#include <string>
#include <vector>
#include "raylib.h"

/**
 * @class TileMap
 * @brief Gestiona la cuadrícula lógica del supramundo, sus colisiones y eventos de compuertas.
 *
 * Leyenda de caracteres en la matriz:
 * - '#' : Árbol o pilar de piedra (bloque sólido impenetrable).
 * - '.' : Pasto o terreno abierto transitable.
 * - 'p' : Camino o sendero adoquinado (transitable, visual diferenciado).
 * - 'W' : Agua profunda (bloque sólido intransitable).
 * - 'H' : Casa o estructura habitable (bloque sólido con techo dibujado).
 * - '1' : Compuerta 1 entre Pradera y Bosque (sólida hasta vencer al Murciélago guardián).
 * - '2' : Compuerta 2 entre Bosque y Ruinas (sólida hasta vencer al Lobo guardián).
 * - 'A'..'F' : Puntos de generación de monstruos (extraídos al inicio y reemplazados por pasto '.').
 */
class TileMap {
public:
    /// Dimensión fija en píxeles de cada casilla en pantalla (32x32 píxeles).
    static constexpr int TILE = 32;

    /// Número total de columnas de la cuadrícula (ancho total: 60 * 32 = 1920 px).
    static constexpr int COLS = 60;

    /// Número total de filas de la cuadrícula (alto total: 32 * 32 = 1024 px).
    static constexpr int ROWS = 32;

    /**
     * @struct Spawn
     * @brief Representa un punto de generación de monstruo encontrado en la matriz inicial.
     */
    struct Spawn {
        char id;    ///< Letra identificadora de la criatura ('A'..'F').
        int col;    ///< Columna de la cuadrícula donde debe aparecer.
        int row;    ///< Fila de la cuadrícula donde debe aparecer.
    };

    /**
     * @brief Constructor del mapa. Inicializa las 32 filas con el diseño de los 4 sectores.
     *
     * Se ejecuta durante la carga del juego o al reiniciar la partida.
     */
    TileMap();

    /**
     * @brief Obtiene el carácter de la casilla en una coordenada (columna, fila) dada.
     *
     * @param col Índice de columna (0 a 59).
     * @param row Índice de fila (0 a 31).
     * @return char Carácter que describe el tipo de casilla. Si las coordenadas están
     *              fuera de los límites del mapa, devuelve '#' (sólido) para confinar entidades.
     */
    char At(int col, int row) const;

    /**
     * @brief Determina si una casilla específica impide el paso físico de entidades.
     *
     * @param col Columna a consultar.
     * @param row Fila a consultar.
     * @return true Si la casilla es un obstáculo ('#', 'W', 'H', o compuerta cerrada '1', '2').
     * @return false Si la casilla es terreno transitable (pasto '.' o camino 'p').
     */
    bool IsSolidTile(int col, int row) const;

    /**
     * @brief Comprueba si un rectángulo colisiona con alguna casilla sólida del mapa.
     *
     * Convierte los extremos del rectángulo a coordenadas de cuadrícula y verifica
     * cada casilla cubierta por el área del cuerpo (detección AABB).
     *
     * @param r Rectángulo en coordenadas mundiales (píxeles) a evaluar.
     * @return true Si alguna porción del rectángulo solapa una casilla sólida.
     * @return false Si el rectángulo se encuentra totalmente sobre espacio libre.
     */
    bool IsBlocked(const Rectangle& r) const;

    /**
     * @brief Mueve un rectángulo aplicando deslizamiento en ejes X e Y de forma independiente.
     *
     * Si el desplazamiento completo en un eje provoca colisión contra un obstáculo sólido,
     * ese componente se cancela manteniendo el movimiento en el eje libre (wall sliding).
     *
     * @param r Referencia al rectángulo de la entidad que será modificado in-situ.
     * @param delta Vector de desplazamiento deseado en píxeles (delta.x, delta.y).
     */
    void Move(Rectangle& r, Vector2 delta) const;

    /**
     * @brief Abre una compuerta mágica en el mapa, transformando sus casillas en camino transitable.
     *
     * @param index Índice de la compuerta (0 para compuerta '1', 1 para compuerta '2').
     *              Es invocado por el Controlador cuando el héroe vence al guardián correspondiente.
     */
    void OpenGate(int index);

    /**
     * @brief Escanea la matriz en busca de letras 'A'..'F', registra sus ubicaciones y las reemplaza por '.'.
     *
     * Llamada por `Game::InitWorld()` durante la inicialización para instanciar a los enemigos.
     *
     * @return std::vector<Spawn> Lista de puntos de generación extraídos de la matriz.
     */
    std::vector<Spawn> ExtractSpawns();

    /**
     * @brief Proporciona acceso de solo lectura a las cadenas de texto que componen el mapa.
     *
     * Utilizado principalmente por `MapView` para el renderizado eficiente de los tiles.
     *
     * @return const std::vector<std::string>& Referencia constante al vector de filas.
     */
    const std::vector<std::string>& Rows() const { return tiles; }

private:
    /// Almacén principal de datos: cada fila es un `std::string` de longitud `COLS` (60 caracteres).
    std::vector<std::string> tiles;
};
