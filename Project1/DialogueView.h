#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "NPC.h"

class DialogueView {
public:
    void Draw(const NPC& npc, const std::vector<std::string>& lines, int currentLine,
        bool isConfirmation = false, int selectedChoice = 0, int hoveredChoice = -1) const;

    Rectangle GetChoiceButtonRect(int index) const;
    int GetHoveredChoice(Vector2 mouse) const;

private:
    void DrawWrappedText(const char* text, int posX, int posY, int maxW, int fontSize, Color color) const;
};
