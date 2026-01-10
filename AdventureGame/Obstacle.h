#pragma once
#include "GameObject.h"
#include "Point.h"
#include "Direction.h"
#include <vector>

class Player;
class Screen;

class Obstacle : public GameObject
{
public:
    // --- Construction ---
    Obstacle(const std::vector<Point>& tiles, ScreenId screenId);

    // --- GameObject Overrides ---
    void drawToBuffer(std::vector<std::string>& buffer) const override;
    bool isAtPosition(int x, int y) const override;
    bool handleExplosionAt(int x, int y) override;
    bool handleCollision(Player& p, const Screen& screen) override;
    std::string getTypeName() const override;
    std::string getSaveData() const override;

    // --- Push Handling ---
    // Called from Player::handleObjectInteractions.
    // Returns true if the move was successful (obstacle moved).
    // Returns false if blocked (player will undo their move).
    bool handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects);

    // --- Debugging ---
    void printDebugInfo() const;

private:
    std::vector<Point> parts;

    // --- Movement Helpers ---
    // Calculate movement vector from a direction.
    static void getDirectionDelta(Direction dir, int& dx, int& dy);

    // Calculate force added by a second player based on strict physical contact.
    int getAssistForce(const Player& helper, const Player& primaryPusher, int pushDx, int pushDy) const;

    // Validate path against walls, active lasers, and other objects.
    bool isPathClear(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const;

    // Apply a movement delta to all obstacle tiles.
    void applyMovement(int dx, int dy);
};
