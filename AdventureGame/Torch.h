#pragma once
#include "GameObject.h"

/*
 * ===================================================================================
 * Class: Torch
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A collectible item that extends the player's light radius in dark rooms.
 * ===================================================================================
 */

class Torch : public GameObject
{
public:
    Torch(int x, int y, char c, ScreenId screen)
        : GameObject(x, y, c, screen, false, false)
    {
    }

    std::string getTypeName() const override { return "TORCH"; }
    // Torch has no extra data beyond X,Y, uses default getSaveData()
};