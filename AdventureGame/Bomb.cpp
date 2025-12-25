#include "Bomb.h"
#include "Player.h"
#include "Screen.h"
#include "Game.h"

Bomb::Bomb(int x, int y, ScreenId screen)
    : GameObject(x, y, '@', screen, true, false) // symbol='@', solid=true, collected=false
{
    active = false;
    exploded = false;
    ticksLeft = COUNTDOWN_TICKS;
}

void Bomb::activate()
{
    active = true;
    exploded = false;
    ticksLeft = COUNTDOWN_TICKS;
}


bool Bomb::handleCollision(Player& p, const Screen& screen)
{
    if (!active && !isCollected())
        p.collectItem(this); // collectible

    return true;
}

bool Bomb::tick()
{
    if (!active)
        return false; 

	// לא פעילה (הוסרה), לא סופרת לאחור
    if (getX() < 0 || getY() < 0)
        return false;

    if (--ticksLeft <= 0)       
    {
        active = false;  
        exploded = true;
        return true;
    }

    return false;             
}

