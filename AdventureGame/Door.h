#pragma once

#include "GameObject.h"

class Player;

/*
    Door:
    Can block movement unless unlocked by a matching key.
    When opened, updates player's current level.
*/
class Door : public GameObject
{
private:
    int doorID;
    bool locked;
    ScreenId leadsTo;

public:
    Door(int x, int y, char c, ScreenId screen,
        int id, ScreenId leadsToScreen, bool isLocked);

    void setDoorProperties(int id, ScreenId targetScreen, bool isLocked);

    bool isLocked() const { return locked; }
    ScreenId getLeadsTo() const { return leadsTo; }
    int getId() const { return doorID; }
    bool unlock(Player& player);                 // Try unlocking using player's key
    bool handleCollision(Player& p, const Screen& screen) override;
};
