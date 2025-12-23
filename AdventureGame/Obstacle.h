#pragma once

#include <vector>
#include <string>

#include "GameObject.h"
#include "Direction.h"

class Obstacle : public GameObject
{
public:
    explicit Obstacle(const std::vector<Point>& tiles, ScreenId screenId);

    bool handleCollision(Player& p, const Screen& screen) override;
    bool handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects);
    bool isAtPosition(int x, int y) const override;

    void drawToBuffer(std::vector<std::string>& buffer) const;

private:
    std::vector<Point> parts;

    static int computeForce(const Player& player);
    static void directionToDelta(Direction dir, int& dx, int& dy);

    bool canMove(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const;
    void moveBy(int dx, int dy);
    bool isAssistingPush(const Player& other, int dx, int dy) const;
};