#pragma once
#include "GameObject.h"

/*
 * ===================================================================================
 * Class: Key
 * -----------------------------------------------------------------------------------
 * Purpose:
 * A collectible item used to unlock Doors.
 * Stores a unique ID to match specific doors.
 * ===================================================================================
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

    void setKeyID(int newID) { keyID = newID; }
    int getKeyID() const { return keyID; }

    std::string getTypeName() const override { return "KEY"; }
    std::string getSaveData() const override {
        // Format: X Y KeyID
        return GameObject::getSaveData() + " " + std::to_string(keyID);
    }
};