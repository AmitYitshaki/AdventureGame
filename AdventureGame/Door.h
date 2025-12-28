#pragma once
#include "GameObject.h"

class Player;

/*
 * ===================================================================================
 * Class: Door
 * -----------------------------------------------------------------------------------
 * Purpose:
 * Acts as a gateway between screens or a barrier within a room.
 *
 * Behavior:
 * - If locked: Blocks movement. Requires a specific Key ID to open.
 * - If unlocked: Transports the player to the target `ScreenId`.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Stores `doorID` (to match keys) and `targetScreen` (destination).
 * ===================================================================================
 */

class Door : public GameObject
{
private:
    int doorID;
    bool locked;
    ScreenId leadsTo;

public:
    Door(int x, int y, char c, ScreenId screen, int id, ScreenId leadsToScreen, bool isLocked);

    void setDoorProperties(int id, ScreenId targetScreen, bool isLocked);
    bool unlock(Player& player);
    bool handleCollision(Player& p, const Screen& screen) override;
};