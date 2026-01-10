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
public:
    // --- Construction ---
    Door(int x, int y, char c, ScreenId screen, int id, ScreenId leadsToScreen, bool isLocked);

    // --- Door State ---
    void setDoorProperties(int id, ScreenId targetScreen, bool isLocked);
    bool unlock(Player& player);

    // --- GameObject Overrides ---
    bool handleCollision(Player& p, const Screen& screen) override;
    std::string getTypeName() const override;
    std::string getSaveData() const override;

private:
    int doorID;
    bool locked;
    ScreenId leadsTo;
};
