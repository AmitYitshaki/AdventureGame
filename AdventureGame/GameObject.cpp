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

void GameObject::setChar(char newC)
{
    // אופציה א': אם יש לך אובייקט point
    point.setChar(newC);
}


// ------------------------------------------------------------
//                        drawing
// ------------------------------------------------------------

void GameObject::drawToBuffer(std::vector<std::string>& buffer) const
{
    // מימוש דיפולטיבי: מצייר את התו של האובייקט במיקום שלו
    int x = point.getX();
    int y = point.getY();

    // בדיקת גבולות בסיסית
    if (y >= 0 && y < (int)buffer.size() &&
        x >= 0 && x < (int)buffer[y].size())
    {
        buffer[y][x] = point.getChar();
    }
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

bool GameObject::handleExplosionAt(int x, int y)
{
    // בדיקה בסיסית: אם הפיצוץ פגע במיקום של האובייקט
    if (isAtPosition(x, y)) {
        return true; // האובייקט מושמד לחלוטין
    }
    return false; // לא נפגע
}