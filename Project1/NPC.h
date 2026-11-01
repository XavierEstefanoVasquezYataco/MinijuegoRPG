#pragma once
#include <string>
#include <vector>
#include "raylib.h"

enum class NPCType {
    MENTOR,
    HEALER,
    GUARD,
    EXPLORER,
    SIGN
};

struct NPC {
    std::string id;
    NPCType type = NPCType::SIGN;
    std::string name;
    Rectangle body{ 0, 0, 24, 24 };
    int art = 0;
    Color bodyColor{ 255, 255, 255, 255 };
    Color lightColor{ 255, 255, 255, 255 };
    bool isSolid = true;
    bool isSign = false;

    bool IsSign() const { return isSign || type == NPCType::SIGN; }

    Vector2 Center() const {
        return { body.x + body.width / 2.0f, body.y + body.height / 2.0f };
    }

    std::vector<std::string> GetDialogue(bool gate1Open, bool gate2Open, bool bossDefeated) const;
};

std::vector<NPC> CreateWorldNPCs();
