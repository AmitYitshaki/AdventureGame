#include "Door.h"
#include "Player.h"
#include "Key.h"
#include <iostream>
#include <string>
#include "Screen.h" 


Door::Door(int x, int y, char c, ScreenId screen,
    int id, ScreenId leadsToScreen, bool isLocked)
    : GameObject(x, y, c, screen, true),
    doorID(id),
    leadsTo(leadsToScreen),
    locked(isLocked)
{
}

void Door::setDoorProperties(int id, ScreenId targetScreen, bool isLocked)
{
    this->doorID = id;
    this->leadsTo = targetScreen;
    this->locked = isLocked;
}

bool Door::unlock(Player& player)
{
    if (!locked) return true;

    if (player.getHeldKeyID() == doorID)
    {
        printDebugBelow("Key MATCH! Unlocking door.");
        locked = false;
        player.removeHeldItem();
        return true;
    }

    printDebugBelow("Locked! Player has KeyID: " + std::to_string(player.getHeldKeyID()) + " | Door needs: " + std::to_string(doorID));
    return false;
}

bool Door::handleCollision(Player& p, const Screen& screen)
{
    // מקרה 1: הדלת פתוחה
    if (!locked)
    {
        printDebugBelow("OPEN! Moving to Level: " + std::to_string((int)leadsTo));

        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    // מקרה 2: הדלת נעולה - מנסים לפתוח
    if (unlock(p))
    {
        printDebugBelow("Unlocked & Moving to Level: " + std::to_string((int)leadsTo));

        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY);
        return true;
    }

    return false; // חוסם מעבר
}