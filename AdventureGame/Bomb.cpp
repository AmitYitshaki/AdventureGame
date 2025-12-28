#include "Bomb.h"
#include "Player.h"

Bomb::Bomb(int x, int y, ScreenId screen)
    : GameObject(x, y, '@', screen, true, false)
{
    active = false;
    ticksLeft = COUNTDOWN_TICKS;
}

void Bomb::activate()
{
    active = true;
    ticksLeft = COUNTDOWN_TICKS;
}

bool Bomb::handleCollision(Player& p, const Screen&)
{
    if (!active && !isCollected())
        p.collectItem(this);
    return true;
}

bool Bomb::tick()
{
    if (!active) return false;
    if (getX() < 0 || getY() < 0) return false;

    // Visual countdown
    if (ticksLeft > 16 && ticksLeft <= 24) setChar('3');
    else if (ticksLeft > 8 && ticksLeft <= 16) setChar('2');
    else if (ticksLeft > 0 && ticksLeft <= 8) setChar('1');

    if (--ticksLeft <= 0)
    {
        active = false;
        return true;
    }
    return false;
}