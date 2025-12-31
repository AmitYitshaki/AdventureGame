#include "Door.h"
#include "Player.h"
#include "Key.h"
#include "Screen.h" 
#include <iostream>
#include <string>
#include "Game.h" //for sound

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
    // If already open, nothing to do
    if (!locked) return true;

    // Check if player has the matching key
    if (player.getHeldKeyID() == doorID)
    {
        locked = false;
        player.removeHeldItem(); // Consume the key
        Game::playSound(300, 200);
        return true;
    }

    return false;
}

bool Door::handleCollision(Player& p, const Screen& screen)
{
    // ---------------------------------------------------------
    // Case 1: The door is LOCKED
    // ---------------------------------------------------------
    if (locked)
    {
        // Try to unlock with the player's key
        if (unlock(p))
        {
            return false;
        }

        // Failed to unlock (no key or wrong key) -> Block movement
        return false;
    }

    // ---------------------------------------------------------
    // Case 2: The door is UNLOCKED (Open)
    // ---------------------------------------------------------

    // Sub-case A: This is a level transition door
    if (leadsTo != screen.getScreenId())
    {
        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY); // Stop movement after teleporting
        return true;
    }

    // Sub-case B: This is an internal door (barrier)
    // Since it is unlocked, remove it from the board so the player can pass
    removeFromGame();

    return true; // Allow movement into the now empty space
}