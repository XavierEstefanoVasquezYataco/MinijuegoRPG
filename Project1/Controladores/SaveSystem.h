#pragma once
#include <vector>

struct SaveData {
    int heroClass = 0;
    int level = 1;
    int hp = 1;
    int maxHp = 1;
    int mp = 0;
    int maxMp = 0;
    int atk = 1;
    int def = 0;
    int potions = 0;
    int exp = 0;
    int expNext = 50;
    float posX = 0.0f;
    float posY = 0.0f;
    int facing = 0;
    bool showMinimap = true;
    std::vector<int> defeatedEnemies;
};

namespace SaveSystem {
    bool Exists();
    bool Save(const SaveData& data);
    bool Load(SaveData& data);
}
