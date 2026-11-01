#pragma once
#include <string>
#include <vector>
#include "raylib.h"

// Leyenda: '#' arbol, '.' pasto, 'p' camino, 'W' agua, 'H' casa,
//          '1','2' compuertas, 'A'..'F' = punto de aparicion de monstruo
class TileMap {
public:
    static constexpr int TILE = 32;
    static constexpr int COLS = 30;
    static constexpr int ROWS = 16;

    struct Spawn { char id; int col; int row; };

    TileMap();

    char At(int col, int row) const;
    bool IsSolidTile(int col, int row) const;
    bool IsBlocked(const Rectangle& r) const;
    void Move(Rectangle& r, Vector2 delta) const;
    void OpenGate(int index);

    // Busca las letras 'A'..'F' en el mapa, las devuelve y las cambia por pasto
    std::vector<Spawn> ExtractSpawns();

    const std::vector<std::string>& Rows() const { return tiles; }

private:
    std::vector<std::string> tiles;
};