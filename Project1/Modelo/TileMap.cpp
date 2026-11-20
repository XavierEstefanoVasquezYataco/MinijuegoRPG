/**
 * @file TileMap.cpp
 * @brief Implementación de la lógica del mapa de casillas y resolución de colisiones AABB.
 *
 * Capa de Arquitectura: Modelo (Model) - Lógica de Escenario.
 *
 * Propósito y Responsabilidades:
 * - Definir la distribución física y geográfica del supramundo de 1920x1024 píxeles (4 sectores).
 * - Implementar la comprobación de límites del mundo y la detección de casillas bloqueantes.
 * - Resolver desplazamientos continuos con desacoplamiento de ejes X e Y para evitar atascos en esquinas.
 * - Manipular la matriz de caracteres para abrir compuertas y extraer generadores de enemigos.
 */

#include "TileMap.h"
#include <cmath>

/**
 * @brief Inicializa el mapa del juego con su cuadrícula fija de 60 columnas por 32 filas.
 *
 * Distribución de sectores:
 * 1. Aldea Segura (cols 0-16): Libre de monstruos, contiene casas, NPCs y caminos empedrados.
 * 2. Pradera Abierta (cols 17-28): Slimes salvajes ('A') y el Murciélago guardián ('B').
 * 3. Bosque Sombrío (cols 29-39): Goblins ('C') y el Lobo guardián ('D').
 * 4. Ruinas Antiguas (cols 40-59): Esqueletos ('E') y el Dragón jefe final ('F').
 *
 * Compuertas lógicas:
 * - '1': En columna 28, filas 14-15 (bloquea el paso hacia el Bosque).
 * - '2': En columna 39, filas 14-15 (bloquea el paso hacia las Ruinas).
 */
TileMap::TileMap() {
    tiles = {
        "############################################################",
        "#................#..........#..........#..........#........#",
        "#.HH...HH........#...A......#...##.....#..HH..E...#...##...#",
        "#.HHp..HH........#.#####....#..####....#..HH......#..####..#",
        "#..pp............#...##.....#...##.....#..........#...##...#",
        "#..p.....HH......#..........#..........#....##....#........#",
        "#..p.WW..HH......#....p..A..#....p..C..#....##....#....E...#",
        "#..p.WW...p......#....p.....#....p.....#....p.....#...##...#",
        "#..pp.....p......#....p.....#....p.....#....p.....#..####..#",
        "#.HHp...WWp......#..ppp.....#...pp.....#...pp.....#...##...#",
        "#.HHp..WWWWp.....#.pp.......#..p.......#..p.......#........#",
        "#...p..WWWWp.....#..p.......#..p...WW..#..p...HH..#..####..#",
        "#...p...WW.p.....#..p...B...#..p..WWWW.#..p...HH..#..#..#..#",
        "#...p......p.....#..p.......#..p..WWWW.#..p.......#..p..#..#",
        "#...pppppppppppppppppppppppp1pppppppppp2ppppppppppppppF.#..#",
        "#...pppppppppppppppppppppppp1pppppppppp2pppppppppppppp..#..#",
        "#...p..HH..p.....#..p.......#..p..WWWW.#..p...E...#..#..#..#",
        "#.HHp..HH..p.....#..pp......#..p..WWWW.#..p.......#..####..#",
        "#.HHp......p.....#...ppp....#..p...WW..#..p...##..#...##...#",
        "#..pp......p.....#.....p....#..p.......#..p..####.#..####..#",
        "#..p..WW...p.....#..A..p....#..p..D....#..p...##..#...##...#",
        "#..p.WWWW..p.....#.....p....#..pp......#..pp......#........#",
        "#..p.WWWW.pp.....#.....p....#...p......#...p......#....E...#",
        "#..p..WW..p......#....pp....#...p..C...#...p......#...##...#",
        "#..p.....pp......#...pp.....#...pp.....#..pp......#..####..#",
        "#..ppppppp.......#..pp......#....p.....#..p.......#...##...#",
        "#................#..p....A..#....p.....#..p.......#........#",
        "#.HH.............#..p.......#....p..C..#..p.......#..####..#",
        "#.HH...HH........#..p.......#....p.....#..p.......#..####..#",
        "#......HH........#..........#..........#..........#........#",
        "#................#..........#..........#..........#........#",
        "############################################################"
    };
}

/**
 * @brief Consulta segura del carácter en una coordenada de celda.
 *
 * Si las coordenadas están fuera del rango válido de la matriz (c < 0 || c >= COLS || r < 0 || r >= ROWS),
 * retorna '#' (muro perimetral sólido). De este modo se garantiza que ninguna entidad
 * pueda escapar del mundo ni provocar un desbordamiento de memoria al indexar el vector.
 */
char TileMap::At(int c, int r) const {
    if (c < 0 || r < 0 || c >= COLS || r >= ROWS) return '#';
    return tiles[r][c];
}

/**
 * @brief Evalúa si el tipo de casilla correspondiente a una coordenada es sólido.
 *
 * Casillas que impiden el paso:
 * - '#': Árboles o pilares.
 * - 'W': Agua profunda.
 * - 'H': Casas y muros de piedra.
 * - '1': Primera compuerta mágica (mientras esté cerrada).
 * - '2': Segunda compuerta mágica (mientras esté cerrada).
 */
bool TileMap::IsSolidTile(int c, int r) const {
    char t = At(c, r);
    return t == '#' || t == 'W' || t == 'H' || t == '1' || t == '2';
}

/**
 * @brief Detección de colisión AABB (Axis-Aligned Bounding Box) contra casillas de la cuadrícula.
 *
 * Algoritmo:
 * 1. Convierte las cuatro esquinas del rectángulo en coordenadas de columna (c0, c1) y fila (r0, r1).
 *    Usa `std::floor` para asegurar que las coordenadas fraccionarias se mapeen a la celda correcta.
 * 2. Itera sobre todas las celdas cubiertas por ese rango rectangular.
 * 3. Si al menos una de las celdas es sólida (`IsSolidTile`), reporta colisión inmediata (`true`).
 *
 * Ejemplo numérico de detección de colisiones de celda:
 * - Entidad con body: x = 100.0f, y = 60.0f, width = 24.0f, height = 24.0f; con TILE = 32 px.
 * - Columna inicial c0 = floor(100.0 / 32) = floor(3.125) = 3.
 * - Columna final   c1 = floor((100.0 + 24.0 - 1.0) / 32) = floor(123.0 / 32) = floor(3.843) = 3.
 * - Fila inicial    r0 = floor(60.0 / 32) = floor(1.875) = 1.
 * - Fila final      r1 = floor((60.0 + 24.0 - 1.0) / 32) = floor(83.0 / 32) = floor(2.593) = 2.
 * - Rango verificado: columnas [3..3], filas [1..2]. Se evalúan únicamente 2 casillas: (col=3, row=1) y (col=3, row=2).
 * - Si alguna de las dos casillas contiene '#', 'W', 'H', '1' o '2', retorna true (bloqueado).
 *
 * @param rc Rectángulo que representa el cuerpo de la entidad en el espacio mundial.
 * @return true Si el rectángulo invade una casilla sólida.
 */
bool TileMap::IsBlocked(const Rectangle& rc) const {
    int c0 = static_cast<int>(std::floor(rc.x / TILE));
    int c1 = static_cast<int>(std::floor((rc.x + rc.width - 1) / TILE));
    int r0 = static_cast<int>(std::floor(rc.y / TILE));
    int r1 = static_cast<int>(std::floor((rc.y + rc.height - 1) / TILE));

    for (int r = r0; r <= r1; ++r) {
        for (int c = c0; c <= c1; ++c) {
            if (IsSolidTile(c, r)) return true;
        }
    }
    return false;
}

/**
 * @brief Desplaza una entidad aplicando deslizamiento contra muros (Wall Sliding).
 *
 * Para ofrecer un control fluido al jugador y a la IA:
 * 1. Prueba el desplazamiento horizontal `delta.x` en una copia temporal del rectángulo.
 *    Si la posición horizontal no está bloqueada, actualiza `rc.x`.
 * 2. Prueba el desplazamiento vertical `delta.y` en una copia temporal del rectángulo.
 *    Si la posición vertical no está bloqueada, actualiza `rc.y`.
 *
 * Esto permite al personaje seguir moviéndose paralelamente a una pared incluso si se
 * presiona una dirección diagonal contra la superficie sólida.
 */
void TileMap::Move(Rectangle& rc, Vector2 d) const {
    // Prueba eje horizontal
    Rectangle t = rc;
    t.x += d.x;
    if (!IsBlocked(t)) rc.x = t.x;

    // Prueba eje vertical
    t = rc;
    t.y += d.y;
    if (!IsBlocked(t)) rc.y = t.y;
}

/**
 * @brief Abre una compuerta mágica sustituyendo su carácter bloqueante por camino transitable 'p'.
 *
 * - Si `index == 0`, busca y reemplaza todas las casillas '1' por 'p'.
 * - Si `index == 1`, busca y reemplaza todas las casillas '2' por 'p'.
 *
 * Esto actualiza físicamente el mapa, permitiendo que `IsSolidTile` retorne `false`
 * y las entidades puedan cruzar libremente hacia la nueva zona.
 */
void TileMap::OpenGate(int index) {
    char id = static_cast<char>('1' + index);
    for (auto& row : tiles) {
        for (auto& ch : row) {
            if (ch == id) ch = 'p';
        }
    }
}

/**
 * @brief Busca marcadores de enemigos ('A'..'F'), registra sus posiciones y limpia las casillas.
 *
 * Proceso:
 * 1. Recorre cada fila y columna de la cuadrícula.
 * 2. Al encontrar un carácter entre 'A' y 'F', guarda una estructura `Spawn` con el id, col y row.
 * 3. Sustituye la casilla en la matriz por pasto transitable ('.'), de modo que el enemigo pueda
 *    moverse libremente sobre su celda de inicio.
 *
 * @return std::vector<TileMap::Spawn> Vector con todos los puntos de aparición para poblar el mundo.
 */
std::vector<TileMap::Spawn> TileMap::ExtractSpawns() {
    std::vector<Spawn> out;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char ch = tiles[r][c];
            if (ch >= 'A' && ch <= 'F') {
                out.push_back({ ch, c, r });
                tiles[r][c] = '.';
            }
        }
    }
    return out;
}
