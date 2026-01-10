#include "Door.h"
#include "Player.h"
#include "Key.h"
#include "Screen.h"
#include <iostream>
#include <string>
#include "Game.h"

// === Construction ===
Door::Door(int x, int y, char c, ScreenId screen,
    int id, ScreenId leadsToScreen, bool isLocked)
    : GameObject(x, y, c, screen, true),
    doorID(id),
    leadsTo(leadsToScreen),
    locked(isLocked)
{
}

// === Door State ===
void Door::setDoorProperties(int id, ScreenId targetScreen, bool isLocked)
{
    doorID = id;
    leadsTo = targetScreen;
    locked = isLocked;
}

bool Door::unlock(Player& player)
{
    // If already open, nothing to do.
    if (!locked) {
        return true;
    }

    // Check if player has the matching key.
    if (player.getHeldKeyID() == doorID)
    {
        locked = false;
        player.removeHeldItem();
        Game::playSound(300, 200);
        return true;
    }

    return false;
}

// === Collision Handling ===
bool Door::handleCollision(Player& p, const Screen& screen)
{
    // Case 1: The door is locked.
    if (locked)
    {
        // Try to unlock with the player's key.
        if (unlock(p))
        {
            return false;
        }

        // Failed to unlock (no key or wrong key) -> block movement.
        return false;
    }

    // Case 2: The door is unlocked (open).

    // Sub-case A: This is a level transition door.
    if (leadsTo != screen.getScreenId())
    {
        p.setCurrentLevel(leadsTo);
        p.setDirection(Direction::STAY); // Stop movement after teleporting.
        return true;
    }

    // Sub-case B: This is an internal door (barrier).
    // Since it is unlocked, remove it from the board so the player can pass.
    removeFromGame();

    return true; // Allow movement into the now-empty space.
}

// === Serialization ===
std::string Door::getTypeName() const { return "DOOR"; }

std::string Door::getSaveData() const
{
    // Format: X Y DoorID TargetScreen Locked(1/0)
    return GameObject::getSaveData() + " " + std::to_string(doorID) + " " +
        std::to_string((int)leadsTo) + " " + (locked ? "1" : "0");
}
