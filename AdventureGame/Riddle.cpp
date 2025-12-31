#include "Riddle.h"
#include "Player.h"

bool Riddle::handleCollision(Player& p, const Screen&)
{
    if (solved) return true;
    p.setInteractingRiddle(this);
    return true;
}