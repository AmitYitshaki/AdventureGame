#pragma once

#include "GameObject.h"
#include "Point.h"
#include "Direction.h" // <--- הוספתי את זה! בלעדיו הוא לא מכיר את Direction
#include <vector>
#include <string>

// Forward declarations
class Player;
class Screen;

class Obstacle : public GameObject
{
public:
    // בנאי
    Obstacle(const std::vector<Point>& tiles, ScreenId screenId);

    // --- Drawing ---
    void drawToBuffer(std::vector<std::string>& buffer) const;

    // --- Movement & Logic ---
    bool isAtPosition(int x, int y) const override;

    // דחיפה והתנגשות
    bool handleCollision(Player& p, const Screen& screen) override;
    bool handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects);

    // --- Explosion Handling ---
    bool eraseBlockAt(int x, int y);

private:
    std::vector<Point> parts;

    // פונקציות עזר פנימיות
    int computeForce(const Player& player);
    static void directionToDelta(Direction dir, int& dx, int& dy);
    bool canMove(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const;
    void moveBy(int dx, int dy);
    bool isAssistingPush(const Player& other, int dx, int dy) const;
};