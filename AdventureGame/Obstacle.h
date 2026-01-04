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
    Obstacle(const std::vector<Point>& tiles, ScreenId screenId);

    // Standard GameObject overrides
    void drawToBuffer(std::vector<std::string>& buffer) const override;
    bool isAtPosition(int x, int y) const override;
    bool handleExplosionAt(int x, int y) override;

    // --- Entry Point ---
    // Called from Player::handleObjectInteractions
    // Returns TRUE if the move was successful (obstacle moved).
    // Returns FALSE if blocked (Player will undo their move).
    bool handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects);

    // Fallback (unused for push logic)
    bool handleCollision(Player& p, const Screen& screen) override;

    void printDebugInfo() const;

    std::string getTypeName() const override;
    std::string getSaveData() const override;

private:
    std::vector<Point> parts;

    // Logic Helper 1: Calculate Movement Vector
    static void getDirectionDelta(Direction dir, int& dx, int& dy);

    // Logic Helper 2: Check Assistance
    // Calculates force added by a second player based on strict physical contact.
    int getAssistForce(const Player& helper, const Player& primaryPusher, int pushDx, int pushDy) const;

    // Logic Helper 3: Path Validation
    // Checks walls, active lasers, and other objects.
    bool isPathClear(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const;

    // Action: Update Coordinates
    void applyMovement(int dx, int dy);

    
    
};