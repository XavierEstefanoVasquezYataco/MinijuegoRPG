#include "TileMap.h"
#include <cmath>

TileMap::TileMap() {
    //  zona 1 (10 col) | borde | zona 2 (9 col) | borde | zona 3 (9 col)
    tiles = {
        "##############################",
        "#........." "#" "........." "#" "........#",
        "#.HH......" "#" "..HH..D.." "#" ".HH...E.#",
        "#.HHp..B.." "#" "..HH..##." "#" ".HH.##..#",
        "#..pp....." "#" ".....##.." "#" "....##..#",
        "#..p..##.." "#" "....p...." "#" "..p.....#",
        "#..p..##A." "#" "....p..C." "#" "..p..E..#",
        "#..p......" "#" "....p...." "#" "..p.....#",
        "#ppppppppp" "1" "ppppppppp" "2" "pppppppp#",
        "#.WWWW...." "#" "....p...." "#" "..p.....#",
        "#WWWWWW..." "#" "....p..HH" "#" "..p.WW..#",
        "#WWWWWW..." "#" "....p..HH" "#" "..p.WW..#",
        "#.WWWW...." "#" "....p...." "#" "........#",
        "#....A...." "#" "..C......" "#" ".....F..#",
        "#........." "#" "........." "#" "........#",
        "##############################"
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