#include "SaveSystem.h"
#include <cmath>
#include <fstream>
#include <string>

namespace {
    // Nombre del archivo en el directorio de trabajo y versión del formato.
    const char* const SAVE_FILE = "savegame.dat";
    const int SAVE_VERSION = 1;
    // Límite de entradas de enemigos que se aceptan al cargar.
    const size_t MAX_ENEMIES = 1000;
}

bool SaveSystem::Save(const SaveData& data) {
    // Abre el archivo para reemplazar el guardado anterior.
    std::ofstream out(SAVE_FILE, std::ios::trunc);
    if (!out) {
        return false;
    }

    // Escribe la versión y los atributos como pares de clave y valor.
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
    // Escribe primero la cantidad de enemigos derrotados y luego sus índices.
    out << "defeated " << data.defeatedEnemies.size();
    for (int index : data.defeatedEnemies) {
        out << ' ' << index;
    }
    out << '\n';
    // Marca el final del guardado y comprueba el estado tras vaciar el búfer.
    out << "end\n";
    out.flush();
    return out.good();
}

bool SaveSystem::Load(SaveData& data) {
    std::ifstream in(SAVE_FILE);
    if (!in) {
        return false;
    }

    // Lee el archivo en una copia temporal. Si falla la carga, los datos de la partida quedan intactos.
    SaveData loaded;
    int version = 0;
    int minimap = 1;
    bool ended = false;
    std::string key;

    // Identifica cada campo por su clave y lee su valor correspondiente.
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
            // Valida la cantidad antes de leer los índices de los enemigos.
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
            // El marcador de cierre confirma que se alcanzó el final previsto del guardado.
            ended = true;
            break;
        }
        else {
            // Ignora el resto de la línea cuando la clave es desconocida.
            std::string rest;
            std::getline(in, rest);
        }

        if (in.fail()) {
            return false;
        }
    }

    // Exige el marcador de cierre y la versión del formato que admite el juego.
    if (!ended || version != SAVE_VERSION) {
        return false;
    }
    // Rechaza valores fuera de rango y coordenadas no finitas.
    if (loaded.heroClass < 0 || loaded.heroClass > 2) return false;
    if (loaded.level < 1 || loaded.maxHp < 1 || loaded.maxMp < 0) return false;
    if (loaded.expNext < 1 || loaded.exp < 0 || loaded.potions < 0) return false;
    if (loaded.atk < 0 || loaded.def < 0) return false;
    if (loaded.facing < 0 || loaded.facing > 3) return false;
    if (!std::isfinite(loaded.posX) || !std::isfinite(loaded.posY)) return false;

    // Ajusta la salud y el maná a sus rangos válidos antes de restaurar el estado.
    if (loaded.hp < 1) loaded.hp = 1;
    if (loaded.hp > loaded.maxHp) loaded.hp = loaded.maxHp;
    if (loaded.mp < 0) loaded.mp = 0;
    if (loaded.mp > loaded.maxMp) loaded.mp = loaded.maxMp;
    loaded.showMinimap = (minimap != 0);

    // Copia los datos validados al objeto data.
    data = loaded;
    return true;
}

bool SaveSystem::Exists() {
    // Usa la misma validación que al cargar una partida.
    SaveData probe;
    return Load(probe);
}
