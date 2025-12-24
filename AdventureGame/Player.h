#pragma once

#include "Point.h"
#include "Direction.h"
#include "ScreenID.h"
#include <vector>

class Screen;
class GameObject;
class Torch;
class Riddle;

/*
    Player:
    Represents one controllable character in the game.
    Handles movement, spring flight, inventory, lives, and HUD info.
*/
class Player
{
private:
    Point point;
    Direction dir = Direction::STAY;
    int hudX;
    int hudY = 0;
    int speed = 1;
	int force = 1;
	int score = 0;
    int live = 3;
    bool flying = false;
    bool loaded = false;
    int springTicksLeft = 0;
    int lastLoadedSpringLength = 0;
    Direction launchDirection = Direction::STAY;
    GameObject* heldItem = nullptr;
    ScreenId currentLevel = ScreenId::ROOM1;

public:
    // --- ctor & dtor ---
    Player(int x, int y, char c, int hudPos)
        : point(x, y, c),
        hudX(hudPos)
    {
    }

    ~Player();

    // Reset all player state for a brand new game
    void resetForNewGame(int x, int y, char c, int hudPos, ScreenId startLevel);


    // --- drawing ---
    void draw() const
    {
        point.draw();
    }


    // --- direction control ---
    void setDirection(Direction direction)
    {
        dir = direction;
    }

    void stopMovement()
    {
        dir = Direction::STAY;
    }

    Direction getDirection() const
    {
        return dir;
    }

    // --- position / char ---
    int getX() const
    {
        return point.getX();
    }

    int getY() const
    {
        return point.getY();
    }

    char getChar() const
    {
        return point.getChar();
    }

    void updatepoint(int x, int y)
    {
        point.setPos(x, y);
    }

    // --- level info ---
    ScreenId getCurrentLevel() const { return currentLevel; }
    void setCurrentLevel(ScreenId id) { currentLevel = id; }
    void initForLevel(int x, int y, char c, ScreenId id);
    void resetStats();

    // --- movement / physics ---
    void move(Screen& screen, std::vector<GameObject*>& gameObjects, const Player* otherPlayer = nullptr);
    void moveFlying(Screen& screen, std::vector<GameObject*>& gameObjects, const Player* otherPlayer = nullptr);
    void launch(int springLen); // start spring effect based on spring length
    int getSpeed() { return speed; }
    bool isFlying() const { return flying; }
    void setFlying(bool canFly) { flying = canFly; }
	int getForce() const { return force; }

    bool isLoaded() const { return loaded; }
    void setLoaded(bool isLoaded) { loaded = isLoaded; }

    void setLaunchDirection(Direction d) { launchDirection = d; }
    Direction getLaunchDirection() const { return launchDirection; }

    void startSpringEffect(int power);   // power = N (spring length)
    void updateSpringEffect();           // decrement ticks; stop at 0
    void stopSpringEffect();             // reset flying state
    int getSpeed() const { return speed; }

    // --- HUD ---
    int getHudX() const { return hudX; }
    int getHudY() const { return hudY; }
    void setHudPosition(int x, int y)
    {
        hudX = x;
        hudY = y;
    }


    // --- inventory / items ---
    bool hasItem() const { return heldItem != nullptr; }
    GameObject* getHeldItem() { return heldItem; }

    bool collectItem(GameObject* item);
    GameObject* dropItemToScreen(ScreenId currentScreenID);
    void removeHeldItem();

    int getHeldKeyID() const;
    bool hasTorch() const;
    bool hasRiddle() const;
    Riddle* getHeldRiddle() const;
    char getItemChar() const;
    // --- life system and score ---
    void decreaseLife();
    int getLive() const { return live; }
	void resetLives() { live = 3; }
	int getScore() const { return score; }
	void increaseScore(int amount) { score += amount; }

    // --- spring stored data ---
    int getLoadedSpringLen() const { return lastLoadedSpringLength; }
    void setLoadedSpringLen(int len) { lastLoadedSpringLength = len; }
};
