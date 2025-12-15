#pragma once

#include <vector>
#include <string>

#include "Point.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Direction.h"

// Forward declarations (avoid heavy includes here)
class Player;
class Screen;

/*
    Spring:
    A multi-segment spring (start–middle–end) that compresses
    when a player moves over it from the correct direction.
    Once fully compressed, it loads and launches the player.
*/
class Spring : public GameObject
{
public:
    // Constructor: spring is composed of 3 points in a row/column
    Spring(const Point& startPt,
        const Point& middlePt,
        const Point& endPt,
        Direction dir,
        ScreenId screenId)
        : GameObject(endPt.getX(), endPt.getY(), 'W', screenId, false, false),
        start(startPt),
        middle(middlePt),
        end(endPt),
        direction(dir)
    {
    }

    // --- Drawing ---
    void drawToBuffer(std::vector<std::string>& buffer) const;

    // --- Collision ---
    bool isAtPosition(int x, int y) const override;
    bool handleCollision(Player& p, const Screen& screen) override;

    // --- Spring Properties ---
    Direction getDirection() const { return direction; }
    int getLength() const { return 3; }   // Current spring supports 3 segments

    // Helper: opposite direction for entry validation
    static Direction oppositeDirection(Direction d);

    void setDirection(Direction newDir);

private:
    Point start;
    Point middle;
    Point end;
    Direction direction;

    void loadSpring(Player& p);  // Trigger player loading & launch preparation
};
