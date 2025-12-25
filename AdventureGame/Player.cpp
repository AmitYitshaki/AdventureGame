#include "Player.h"
#include "Screen.h"
#include "GameObject.h"
#include "Key.h"
#include "Torch.h"
#include "Riddle.h"
#include "Bomb.h"
#include "Door.h"
#include "Game.h"
#include <vector>

/*
    Player.cpp
    Implements normal movement, flying (spring launch),
    item handling, collisions, and life system.
*/

Player::~Player()
{
	heldItem = nullptr; // Do not delete held item; managed by Game
}

void Player::resetForNewGame(int x, int y, char c, int hudPos, ScreenId startLevel)
{
    // Position & char
    point.setPos(x, y);   // לפי השינוי שעשית (setPos)
    point.setChar(c);

    // HUD location
    hudX = hudPos;
    hudY = 0;

    // Basic movement state
    dir = Direction::STAY;
    speed = 1;

    // Life & level
    live = 3;
    currentLevel = startLevel;

    // Spring / flying state
    flying = false;
    loaded = false;
    springTicksLeft = 0;
    lastLoadedSpringLength = 0;
    launchDirection = Direction::STAY;

    // Inventory
    heldItem = nullptr;
}



/* ----------------------------------------------------
                    NORMAL MOVEMENT
   ---------------------------------------------------- */

void Player::move(Screen& screen, std::vector<GameObject*>& gameObjects)
{
	if (point.getX() == -1 || point.getY() == -1) //if player is off-screen
    {
        return;
    }
	if (isLoaded()) //player is in loaded state
    {
        return;
    }
    // If flying due to spring effect → redirect movement
    if (isFlying())
    {
        moveFlying(screen, gameObjects);
        return;
    }

    int dx = 0, dy = 0;

    switch (dir)
    {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    case Direction::STAY:  return;
    }

    Point next(point.getX() + dx, point.getY() + dy, ' ');

    // Wall collision stopping regular movement
    if (screen.isWall(next))
    {
        stopMovement();
        return;
    }

    // Move
    point.move(dx, dy);

    // Handle collisions with game objects
    ScreenId currentScreenId = screen.getScreenId();
    int px = point.getX();
    int py = point.getY();

    for (auto obj : gameObjects)
    {
        if (obj->getScreenId() != currentScreenId)
            continue;

        if (obj->isAtPosition(px, py) && !obj->isCollected())
        {
            bool allowed = obj->handleCollision(*this, screen);

            if (!allowed)
            {
                // Undo movement
                point.move(-dx * speed, -dy * speed);
                dir = Direction::STAY;
            }
        }
    }
}

/* ----------------------------------------------------
                SPRING / FLYING MOVEMENT
   ---------------------------------------------------- */

void Player::moveFlying(Screen& screen, std::vector<GameObject*>& gameObjects)
{
    int dx = 0, dy = 0;

    // Base launch direction from spring
    switch (launchDirection)
    {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    default:
        return;
    }

    // Allow vertical deviation inputs while flying
    if (dir == Direction::UP)        dy -= 1;
    else if (dir == Direction::DOWN) dy += 1;

    Point next(point.getX() + dx, point.getY() + dy, ' ');

    // Wall collision stops spring effect
    if (screen.isWall(next))
    {
        stopSpringEffect();
        return;
    }

    // Move
    point.move(dx, dy);

    // Handle collisions with other objects
    ScreenId currentScreenId = screen.getScreenId();
    int px = point.getX();
    int py = point.getY();

    for (auto obj : gameObjects)
    {
        if (obj->getScreenId() != currentScreenId)
            continue;

        if (obj->isAtPosition(px, py) && !obj->isCollected())
        {
            bool allowed = obj->handleCollision(*this, screen);

            if (!allowed)
            {
                // Undo movement and end spring effect
                point.move(-dx, -dy);
                stopSpringEffect();
                return;
            }
        }
    }
}

/* ----------------------------------------------------
                    SPRING LAUNCH
   ---------------------------------------------------- */

void Player::launch(int springLen)
{
    // springLen = N, startSpringEffect computes N^2
    startSpringEffect(springLen);
}

void Player::startSpringEffect(int springLen)
{
    // Duration = N^2 game cycles
    springTicksLeft = springLen * springLen;

    flying = true;
    loaded = false;
    dir = launchDirection; // lock initial launch direction
}

void Player::updateSpringEffect()
{
    if (!flying)
        return;

    if (springTicksLeft > 0)
        springTicksLeft--;

    if (springTicksLeft <= 0)
        stopSpringEffect();
}

void Player::stopSpringEffect()
{
    flying = false;
    springTicksLeft = 0;
    speed = 1;

    launchDirection = Direction::STAY;
    dir = Direction::STAY;
}

/* ----------------------------------------------------
                    ITEM HANDLING
   ---------------------------------------------------- */

bool Player::collectItem(GameObject* item)
{
    if (!hasItem())
    {
        heldItem = item;
        item->Collected();

        // Attach to HUD position
        item->setPosition(hudX, hudY);

        Game::setStatusMessage(std::string("Collected item '") + item->getChar() + "'");
        return true;
    }
    return false;
}

GameObject* Player::dropItemToScreen(ScreenId currentScreenID)
{
    if (!hasItem())
        return nullptr;

    GameObject* itemToDrop = heldItem;

    itemToDrop->setPosition(point.getX(), point.getY());
    itemToDrop->setScreenId(currentScreenID);
    itemToDrop->drop();

    if (itemToDrop->getChar() == '@')
    {
        static_cast<Bomb*>(itemToDrop)->activate();
        Game::setStatusMessage("Bomb activated! RUN!"); 
    }

    else 
        Game::setStatusMessage(std::string("Dropped item '") + itemToDrop->getChar() + "'");

    heldItem = nullptr;
    return itemToDrop;
}

void Player::removeHeldItem()
{
    if (heldItem)
    {
        heldItem->removeFromGame();
        heldItem = nullptr;
    }
}

/* ----------------------------------------------------
                    INVENTORY QUERIES
   ---------------------------------------------------- */

int Player::getHeldKeyID() const
{
    if (heldItem)
    {
        if (auto k = dynamic_cast<Key*>(heldItem))
            return k->getKeyID();
    }
    return -1;
}

bool Player::hasTorch() const
{
    return (heldItem && dynamic_cast<Torch*>(heldItem) != nullptr);
}

bool Player::hasRiddle() const
{
    return (heldItem && dynamic_cast<Riddle*>(heldItem) != nullptr);
}

Riddle* Player::getHeldRiddle() const
{
    if (hasRiddle())
        return dynamic_cast<Riddle*>(heldItem);

    return nullptr;
}

char Player::getItemChar() const
{
    return heldItem ? heldItem->getChar() : ' ';
}

/* ----------------------------------------------------
                        LIFE SYSTEM
   ---------------------------------------------------- */

void Player::decreaseLife()
{
    if (live > 0)
        live--;
}
