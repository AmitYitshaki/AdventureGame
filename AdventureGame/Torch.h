#pragma once

#include "GameObject.h"
#include "ScreenID.h"

/*
    Torch:
    Simple collectible item that extends the player's
    light radius in dark rooms when held.
*/
class Torch : public GameObject
{
public:
    Torch(int x, int y, char c, ScreenId screen)
        : GameObject(x, y, c, screen, false, false)
    {
    }
};
