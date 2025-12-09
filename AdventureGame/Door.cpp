#include "Door.h"
#include "Player.h"
#include "Key.h"

/*
    Door.cpp
    Implements unlocking logic and custom collision behavior.
*/

Door::Door(int x, int y, char c, ScreenId screen,
    int id, ScreenId leadsToScreen, bool isLocked)
    : GameObject(x, y, c, screen, true),
    doorID(id),
    leadsTo(leadsToScreen),
    locked(isLocked)
{
}

bool Door::unlock(Player& player)
{
    if (!locked)
        return true;

    if (player.getHeldKeyID() == doorID)
    {
        locked = false;
        player.removeHeldItem();
        return true;
    }

    return false;
}

bool Door::handleCollision(Player& p, const Screen& screen)
{
    // Door open → allow passage
    if (!locked)
    {
        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    // Door locked: try unlocking
    if (unlock(p))
    {
        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    return false; // Block movement
}
