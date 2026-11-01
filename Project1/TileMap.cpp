#include "TileMap.h"
#include <cmath>

TileMap::TileMap() {
    // Mapa ampliado de 60 columnas x 32 filas (1920x1024 pixeles)
    // 4 zonas: Aldea Segura (cols 0-16), Pradera Abierta (cols 17-28),
    //          Bosque Sombrio (cols 29-39), Ruinas Antiguas (cols 40-59)
    // Compuertas: '1' entre Pradera y Bosque (col 28, filas 14-15)
    //             '2' entre Bosque y Ruinas (col 39, filas 14-15)
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

char TileMap::At(int c, int r) const {
    if (c < 0 || r < 0 || c >= COLS || r >= ROWS) return '#';
    return tiles[r][c];
}

bool TileMap::IsSolidTile(int c, int r) const {
    char t = At(c, r);
    return t == '#' || t == 'W' || t == 'H' || t == '1' || t == '2';
}

bool TileMap::IsBlocked(const Rectangle& rc) const {
    int c0 = static_cast<int>(std::floor(rc.x / TILE));
    int c1 = static_cast<int>(std::floor((rc.x + rc.width - 1) / TILE));
    int r0 = static_cast<int>(std::floor(rc.y / TILE));
    int r1 = static_cast<int>(std::floor((rc.y + rc.height - 1) / TILE));
    for (int r = r0; r <= r1; ++r)
        for (int c = c0; c <= c1; ++c)
            if (IsSolidTile(c, r)) return true;
    return false;
}

void TileMap::Move(Rectangle& rc, Vector2 d) const {
    Rectangle t = rc;
    t.x += d.x;
    if (!IsBlocked(t)) rc.x = t.x;
    t = rc;
    t.y += d.y;
    if (!IsBlocked(t)) rc.y = t.y;
}

void TileMap::OpenGate(int index) {
    char id = static_cast<char>('1' + index);
    for (auto& row : tiles)
        for (auto& ch : row)
            if (ch == id) ch = 'p';
}

std::vector<TileMap::Spawn> TileMap::ExtractSpawns() {
    std::vector<Spawn> out;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            char ch = tiles[r][c];
            if (ch >= 'A' && ch <= 'F') {
                out.push_back({ ch, c, r });
                tiles[r][c] = '.';
            }
        }
    return out;
}
