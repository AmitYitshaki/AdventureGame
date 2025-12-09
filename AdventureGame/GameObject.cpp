#include "GameObject.h"
#include "Player.h"

/*
    GameObject.cpp
    Implements basic object behavior:
    collision rules, inventory transitions,
    and world removal.
*/

// ------------------------------------------------------------
//                      WORLD BEHAVIOR
// ------------------------------------------------------------

void GameObject::dropToScreen(int x, int y)
{
    // Place the object in world coordinates
    point.setPos(x, y);
    collected = false;
}

void GameObject::removeFromGame()
{
    // Soft delete: move object off-screen
    setPosition(-1, -1);
}

// ------------------------------------------------------------
//                        COLLISION
// ------------------------------------------------------------

bool GameObject::handleCollision(Player& p, const Screen& screen)
{
    // Case 1: Non-solid → treat as pickup item
    if (!isSolid())
    {
        if (!isCollected())
            p.collectItem(this);

        return true; // Allow stepping onto object
    }

    // Case 2: Solid → block movement
    // Derived classes (e.g., Door) override this for custom behavior.
    return false;
}
