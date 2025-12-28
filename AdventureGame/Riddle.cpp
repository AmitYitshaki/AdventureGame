#include "Riddle.h"
#include "Player.h"

bool Riddle::handleCollision(Player& p, const Screen&)
{
    if (isCollected()) return true;
    if (p.hasItem()) return false;

    p.collectItem(this);
    Collected();
    removeFromGame();
    return true;
}