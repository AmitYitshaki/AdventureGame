#pragma once
#include "GameObject.h"

class Key : public GameObject
{
private:
    int keyID;

public:
    Key(int x, int y, char c, ScreenId screen, int id)
        : GameObject(x, y, c, screen, false, false), // solid=false, static=false
        keyID(id)
    {
    }

    void setKeyID(int newID) { keyID = newID; }
    int getKeyID() const { return keyID; }

    // מחקנו את handleCollision!
    // המפתח ישתמש בפונקציה של האבא (GameObject) שתקרא ל-player.collectItem(this)
};