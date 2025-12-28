#pragma once
#include "GameObject.h"
#include <vector>

/*
 * ===================================================================================
 * Class: Switch
 * -----------------------------------------------------------------------------------
 * Purpose:
 * An interactive floor trigger that toggles the state of connected objects.
 *
 * Behavior:
 * - Toggles state (Active/Inactive) when a player steps on it.
 * - Changes visual appearance ('/' vs '\').
 * - Sends signals to all linked targets (e.g., turns Lasers on/off).
 *
 * Implementation:
 * - Inherits from GameObject.
 * - Holds a list of pointers to target GameObjects (Observer pattern lite).
 * ===================================================================================
 */

class Switch : public GameObject
{
private:
    bool active;
    std::vector<GameObject*> targets;

public:
    Switch(int x, int y, ScreenId screen);
    void addTarget(GameObject* obj);
    bool handleCollision(Player& p, const Screen& screen) override;
    void toggle();
};