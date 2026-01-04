#pragma once
#include <vector>
#include <string>
#include "Point.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Direction.h"

class Player;
class Screen;

/*
 * ===================================================================================
 * Class: Spring
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A dynamic object that launches the player into the air ("Flight Mode").
 *
 * Behavior:
 * - Multi-segment object (Base 'W' + Tail 'w').
 * - Compresses when stepped on from the correct direction.
 * - Calculates launch power based on its length.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Contains logic to rebuild itself if rotated or damaged.
 * ===================================================================================
 */

class Spring : public GameObject
{
public:
    Spring(const Point& basePosition, Direction dir, ScreenId screenId, int length);

    void drawToBuffer(std::vector<std::string>& buffer) const override;
    bool isAtPosition(int x, int y) const override;
    bool handleCollision(Player& p, const Screen& screen) override;
    bool handleExplosionAt(int x, int y) override;

    void setDirection(Direction newDir);
    Direction getDirection() const { return direction; }
    Direction getOppositeDirection() const { return oppositeDir; }
    int getLength() const { return (int)parts.size(); }
    void rebuild(Direction dir, int length);

    std::string getTypeName() const override;
    std::string getSaveData() const override;

private:
    std::vector<Point> parts;
    Direction direction;
    Direction oppositeDir;
    static void getDelta(Direction d, int& dx, int& dy);
    static Direction oppositeDirection(Direction d);
};