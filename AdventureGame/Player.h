#pragma once
#include "Point.h"
#include "Direction.h"
#include "ScreenID.h"
#include <vector>

class Screen;
class GameObject;
class Riddle;

/*
 * ===================================================================================
 * Class: Player
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Represents a user-controlled character. Handles movement physics, input response,
 * inventory management, and life/score stats.
 *
 * Behavior:
 * - Moves across the screen based on input.
 * - Interacts with GameObjects (collects items, pushes obstacles).
 * - Implements special movement mechanics like "Spring Flight".
 * - Manages held items (Keys, Torch, etc.).
 *
 * Implementation:
 * - Encapsulates physics state (speed, force, direction).
 * - Contains logic for collision resolution with the static map and dynamic objects.
 * ===================================================================================
 */

class Player
{
public:
    Player(int x, int y, char c, int hudPos) : point(x, y, c), hudX(hudPos) {}
    ~Player();

    void resetForNewGame(int x, int y, char c, int hudPos, ScreenId startLevel);
    void initForLevel(int x, int y, char c, ScreenId id);

    // Logic
    void move(Screen& screen, std::vector<GameObject*>& gameObjects, Player* otherPlayer = nullptr);
    void launch(int springLen);
    void stopMovement();

    // State
    bool isFlying() const { return flying; }
    void setFlying(bool canFly) { flying = canFly; }
    bool isLoaded() const { return loaded; }
    void setLoaded(bool isLoaded) { loaded = isLoaded; }

    // Inventory
    bool collectItem(GameObject* item);
    GameObject* dropItemToScreen(ScreenId currentScreenID);
    void removeHeldItem();
    bool hasItem() const { return heldItem != nullptr; }
    bool hasTorch() const;
    bool hasRiddle() const;
    Riddle* getHeldRiddle() const;
    int getHeldKeyID() const;
    char getItemChar() const;

    // Stats
    int getLive() const { return live; }
    int getScore() const { return score; }
    void decreaseLife() { if (live > 0) live--; }
    void addScore(int amount) { score += amount; }
    void resetStats();
    void saveState() { savedLife = live; savedScore = score; }
    void restoreState();

    // Accessors
    int getX() const { return point.getX(); }
    int getY() const { return point.getY(); }
    char getChar() const { return point.getChar(); }
    Direction getDirection() const { return dir; }
	Direction getLaunchDirection() const { return launchDirection; }
    void setDirection(Direction d) { dir = d; }
    int getHudX() const { return hudX; }
    int getHudY() const { return hudY; }
    void setHudPosition(int x, int y) { hudX = x; hudY = y; }
    ScreenId getCurrentLevel() const { return currentLevel; }
    void setCurrentLevel(ScreenId id) { currentLevel = id; }
    bool isHoldingItem(const GameObject* obj) const { return heldItem == obj; }
    void forceDropItem() {heldItem = nullptr; } // Disconnect without triggering logic

    // Physics
    int getSpeed() const { return speed; }
    int getForce() const { return force; }
    void updatepoint(int x, int y) { point.setPos(x, y); }
    void setLaunchDirection(Direction d) { launchDirection = d; }
    int getLoadedSpringLen() const { return lastLoadedSpringLength; }
    void setLoadedSpringLen(int len) { lastLoadedSpringLength = len; }
    void startSpringEffect(int power);
    void updateSpringEffect();
    void stopSpringEffect();

private:
    void calculateMovementDelta(int& dx, int& dy) const;
    bool isBlockedByStatic(int x, int y, const Screen& screen, Player* otherPlayer) const;
    bool handleObjectInteractions(const Screen& screen, std::vector<GameObject*>& gameObjects, Player* otherPlayer);
    void handleStop();

    Point point;
    Direction dir = Direction::STAY;
    int hudX;
    int hudY = 0;
    int speed = 1;
    int force = 1;
    int score = 0;
    int live = 3;

    // Flying State
    bool flying = false;
    bool loaded = false;
    int springTicksLeft = 0;
    int lastLoadedSpringLength = 0;
    Direction launchDirection = Direction::STAY;

    GameObject* heldItem = nullptr;
    ScreenId currentLevel = ScreenId::ROOM1;

    int savedLife = 3;
    int savedScore = 0;
};