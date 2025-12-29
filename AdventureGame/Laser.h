#pragma once
#include "GameObject.h"

/*
 * ===================================================================================
 * Class: Laser
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A hazard barrier that blocks movement. Controlled remotely by Switches.
 *
 * Behavior:
 * - Active state: Draws a beam ('-' or '|') and blocks player.
 * - Inactive state: Invisible and passable.
 * - Responds to `receiveSignal()` events from a Switch.
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Overrides `drawToBuffer` to handle visibility toggling.
 * ===================================================================================
 */

class Laser : public GameObject
{
private:
    bool active;
    char originalSymbol;

public:
    Laser(int x, int y, char c, ScreenId screen);
    void receiveSignal(bool state) override;
    bool handleCollision(Player& p, const Screen& screen) override;
	bool isActive() const { return active; }
};