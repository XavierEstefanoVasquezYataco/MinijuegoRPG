#pragma once
#include <vector>
#include "raylib.h"
#include "TileMap.h"
#include "Player.h"

class MapView {
public:
    void Load();     // llamar despues de InitWindow
    void Unload();

    void DrawWorld(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies) const;
    void DrawHUD(const Player& player) const;
    void DrawMinimap(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies) const;

    void DrawPause(int selected) const;
    int GetPauseButton(Vector2 mouse) const;   // -1 si ninguno

private:
    Texture2D heroSheet{};
    bool hasSheet = false;

    void DrawTile(const TileMap& map, int c, int r) const;
    void DrawPlayer(const Player& p) const;
    void DrawEnemy(const Enemy& e) const;
    Rectangle PauseButton(int i) const;
};