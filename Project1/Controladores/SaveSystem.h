#pragma once
#include <vector>

// Estado de la partida que se guarda y se restaura al continuar.
struct SaveData {
    // Identificador numérico de la clase del héroe.
    int heroClass = 0;
    // Nivel actual y valores iniciales de salud (HP) y maná (MP).
    int level = 1;
    int hp = 1;
    int maxHp = 1;
    int mp = 0;
    int maxMp = 0;
    // Ataque, defensa y cantidad de pociones disponibles.
    int atk = 1;
    int def = 0;
    int potions = 0;
    // Experiencia acumulada y requisito para subir de nivel.
    int exp = 0;
    int expNext = 50;
    // Posición del héroe en el mapa y dirección hacia la que mira.
    float posX = 0.0f;
    float posY = 0.0f;
    int facing = 0;
    // Preferencia de visibilidad del minimapa.
    bool showMinimap = true;
    // Índices de los enemigos derrotados que se restauran al cargar.
    std::vector<int> defeatedEnemies;
};

namespace SaveSystem {
    // Comprueba si el archivo contiene una partida que se puede cargar.
    bool Exists();
    // Escribe el estado de la partida y devuelve si la escritura se completó.
    bool Save(const SaveData& data);
    // Restaura el estado si el archivo supera la validación.
    bool Load(SaveData& data);
}
