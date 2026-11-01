#pragma once
#include <vector>
#include "raylib.h"
#include "TileMap.h"
#include "Player.h"
#include "NPC.h"

class MapView {
public:
    void Load();     // llamar despues de InitWindow
    void Unload();

    void DrawWorld(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies, const std::vector<NPC>& npcs,
        const Camera2D& camera, const NPC* nearbyNpc = nullptr) const;
    void DrawHUD(const Player& player) const;
    void DrawMinimap(const TileMap& map, const Player& player,
        const std::vector<Enemy>& enemies, const Camera2D& camera) const;

    void DrawPause(int selected) const;
    int GetPauseButton(Vector2 mouse) const;   // -1 si ninguno
    Rectangle PauseButton(int i) const;

private:
    Texture2D heroSheet{};
    bool hasSheet = false;

    void DrawTile(const TileMap& map, int c, int r) const;
    void DrawPlayer(const Player& p) const;
    void DrawEnemy(const Enemy& e) const;
    void DrawNPC(const NPC& npc, bool isPromptTarget) const;
};
