#include "Riddle.h"
#include "Player.h"

// טיפול בהתנגשות: השחקן אוסף את החידה
bool Riddle::handleCollision(Player& p, const Screen& /*screen*/)
{
    if (isCollected()) return true;
    if (p.hasItem()) return false;

    // השחקן אוסף את החידה
    p.collectItem(this);
    Collected();
    removeFromGame();

    return true;
}