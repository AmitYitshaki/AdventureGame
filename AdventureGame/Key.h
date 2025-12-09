#pragma once

#include "GameObject.h"

/*
    Key:
    Simple collectible that unlocks a specific door.
*/
class Key : public GameObject
{
private:
    int keyID;

public:
    Key(int x, int y, char c, ScreenId screen, int id)
        : GameObject(x, y, c, screen, false, false),
        keyID(id)
    {
    }

    int getKeyID() const { return keyID; }
};
