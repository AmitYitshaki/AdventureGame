#pragma once
#include "GameObject.h"
#include "Point.h"
#include "Direction.h"
#include <vector>

class Player;
class Screen;

/*
 * ===================================================================================
 * Class: Obstacle
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Represents a large, movable block or barrier composed of multiple tiles.
 *
 * Behavior:
 * - Blocks player movement unless pushed with sufficient force.
 * - Supports cooperative pushing (two players pushing together).
 * - Can be destroyed piece-by-piece by explosions.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Maintains a list of `Point`s representing its shape/parts.
 * - Overrides `handleCollision` to implement the push physics logic.
 * ===================================================================================
 */

class Obstacle : public GameObject
{
public:
    Obstacle(const std::vector<Point>& tiles, ScreenId screenId);

    void drawToBuffer(std::vector<std::string>& buffer) const override;
    bool isAtPosition(int x, int y) const override;

    bool handleCollision(Player& p, const Screen& screen) override;
    bool handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects);
    bool handleExplosionAt(int x, int y) override;

private:
    std::vector<Point> parts;

    int computeForce(const Player& player) const;
    static void directionToDelta(Direction dir, int& dx, int& dy);
    bool canMove(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const;
    void moveBy(int dx, int dy);
    bool isAssistingPush(const Player& other, int dx, int dy) const;
};