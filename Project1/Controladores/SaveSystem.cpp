#include "SaveSystem.h"
#include <cmath>
#include <fstream>
#include <string>

namespace {
    const char* const SAVE_FILE = "savegame.dat";
    const int SAVE_VERSION = 1;
    const size_t MAX_ENEMIES = 1000;
}

bool SaveSystem::Save(const SaveData& data) {
    std::ofstream out(SAVE_FILE, std::ios::trunc);
    if (!out) {
        return false;
    }

    out << "version " << SAVE_VERSION << '\n';
    out << "heroClass " << data.heroClass << '\n';
    out << "level " << data.level << '\n';
    out << "hp " << data.hp << '\n';
    out << "maxHp " << data.maxHp << '\n';
    out << "mp " << data.mp << '\n';
    out << "maxMp " << data.maxMp << '\n';
    out << "atk " << data.atk << '\n';
    out << "def " << data.def << '\n';
    out << "potions " << data.potions << '\n';
    out << "exp " << data.exp << '\n';
    out << "expNext " << data.expNext << '\n';
    out << "posX " << data.posX << '\n';
    out << "posY " << data.posY << '\n';
    out << "facing " << data.facing << '\n';
    out << "showMinimap " << (data.showMinimap ? 1 : 0) << '\n';
    out << "defeated " << data.defeatedEnemies.size();
    for (int index : data.defeatedEnemies) {
        out << ' ' << index;
    }
    out << '\n';
    out << "end\n";
    out.flush();
    return out.good();
}

bool SaveSystem::Load(SaveData& data) {
    std::ifstream in(SAVE_FILE);
    if (!in) {
        return false;
    }

    SaveData loaded;
    int version = 0;
    int minimap = 1;
    bool ended = false;
    std::string key;

    while (in >> key) {
        if (key == "version") in >> version;
        else if (key == "heroClass") in >> loaded.heroClass;
        else if (key == "level") in >> loaded.level;
        else if (key == "hp") in >> loaded.hp;
        else if (key == "maxHp") in >> loaded.maxHp;
        else if (key == "mp") in >> loaded.mp;
        else if (key == "maxMp") in >> loaded.maxMp;
        else if (key == "atk") in >> loaded.atk;
        else if (key == "def") in >> loaded.def;
        else if (key == "potions") in >> loaded.potions;
        else if (key == "exp") in >> loaded.exp;
        else if (key == "expNext") in >> loaded.expNext;
        else if (key == "posX") in >> loaded.posX;
        else if (key == "posY") in >> loaded.posY;
        else if (key == "facing") in >> loaded.facing;
        else if (key == "showMinimap") in >> minimap;
        else if (key == "defeated") {
            size_t count = 0;
            in >> count;
            if (in.fail() || count > MAX_ENEMIES) {
                return false;
            }
            for (size_t i = 0; i < count; ++i) {
                int index = -1;
                in >> index;
                loaded.defeatedEnemies.push_back(index);
            }
        }
        else if (key == "end") {
            ended = true;
            break;
        }
        else {
            std::string rest;
            std::getline(in, rest);
        }

        if (in.fail()) {
            return false;
        }
    }

    if (!ended || version != SAVE_VERSION) {
        return false;
    }
    if (loaded.heroClass < 0 || loaded.heroClass > 2) return false;
    if (loaded.level < 1 || loaded.maxHp < 1 || loaded.maxMp < 0) return false;
    if (loaded.expNext < 1 || loaded.exp < 0 || loaded.potions < 0) return false;
    if (loaded.atk < 0 || loaded.def < 0) return false;
    if (loaded.facing < 0 || loaded.facing > 3) return false;
    if (!std::isfinite(loaded.posX) || !std::isfinite(loaded.posY)) return false;

    if (loaded.hp < 1) loaded.hp = 1;
    if (loaded.hp > loaded.maxHp) loaded.hp = loaded.maxHp;
    if (loaded.mp < 0) loaded.mp = 0;
    if (loaded.mp > loaded.maxMp) loaded.mp = loaded.maxMp;
    loaded.showMinimap = (minimap != 0);

    data = loaded;
    return true;
}

bool SaveSystem::Exists() {
    SaveData probe;
    return Load(probe);
}
