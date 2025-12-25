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

    // הגנה: אם הפצצה כבר הוסרה מהלוח, לא נחשב
    if (getX() < 0 || getY() < 0)
        return false;

    // --- שינוי ויזואלי: ספירה לאחור ---
    // רק ב-9 הטיקים האחרונים נציג מספרים (9..1)
    if (ticksLeft > 16 && ticksLeft <= 24) {
        setChar('3');
    }
    else if (ticksLeft > 8 && ticksLeft <= 16 ) {
        setChar('2');
    }
    else if (ticksLeft > 0 && ticksLeft <= 8) {
        setChar('1');
    }

    // הפחתת הטיימר
    if (--ticksLeft <= 0)
    {
        active = false;
        exploded = true;
        return true; // הפצצה התפוצצה בטיק הזה!
    }

    return false;
}

