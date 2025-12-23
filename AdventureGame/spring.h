#pragma once

#include <vector>
#include <string>

#include "Point.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Direction.h"

// Forward declarations
class Player;
class Screen;

/*
    Spring:
    A multi-segment spring (start–middle–end) that compresses
    when a player moves over it from the correct direction.
*/
class Spring : public GameObject
{
public:
    // Constructor
    Spring(const Point& startPt,
        const Point& middlePt,
        const Point& endPt,
        Direction dir,
        ScreenId screenId,
        const int springLength);

    // --- Drawing ---
    void drawToBuffer(std::vector<std::string>& buffer) const;

    // --- Collision ---
    bool isAtPosition(int x, int y) const override;
    bool handleCollision(Player& p, const Screen& screen) override;

    // --- Spring Properties ---
    Direction getDirection() const { return direction; }
    Direction getOppositeDirection() const { return oppositeDir; }
    int getLength() const { return springLength; }
	void setLength(int length) { springLength = length; }

    // Helper: calculate opposite direction
    static Direction oppositeDirection(Direction d);

    void setDirection(Direction newDir);

private:
    Point start;
    Point middle;
    Point end;
    Direction direction;
    int springLength;
    // Member to cache the opposite direction (optimization)
    Direction oppositeDir;

    void loadSpring(Player& p);
};