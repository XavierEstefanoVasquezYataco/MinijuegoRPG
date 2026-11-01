#pragma once
#include "raylib.h"
#include "GameState.h"

class TitleView {
public:
    TitleView();

    void DrawTitle(int selected, bool continueAvailable, int hovered = -1) const;
    void DrawClassSelect(int selectedClass, int hoveredClass = -1, bool backHovered = false, bool startHovered = false) const;
    void DrawControls(bool backHovered = false) const;

    int GetTitleButton(Vector2 point) const;
    int GetClassCard(Vector2 point) const;
    bool IsOverBackButton(Vector2 point, GameState state) const;
    bool IsOverStartButton(Vector2 point) const;

    Rectangle GetTitleButtonRect(int index) const;
    Rectangle GetClassCardRect(int index) const;
    Rectangle GetBackButtonRect(GameState state) const;
    Rectangle GetStartButtonRect() const;

private:
    Rectangle titleButtons[4];
    Rectangle classCards[3];

    void DrawDecoratedFrame(Rectangle r, Color border, Color fill) const;
};
