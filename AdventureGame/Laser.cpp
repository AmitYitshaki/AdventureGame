#include "Laser.h"

Laser::Laser(int x, int y, char c, ScreenId screen)
    : GameObject(x, y, c, screen, true), // solid = true
    active(true),
    originalSymbol(c) // שומרים את הצורה המקורית
{
}

void Laser::receiveSignal(bool switchState)
{
    // הלוגיקה ההפוכה:
    // מתג דולק (true)  --> לייזר כבוי (false)
    // מתג כבוי (false) --> לייזר דולק (true)
    active = !switchState;

    if (active) {
        // אם הדלקנו חזרה - משחזרים את הסימן המקורי (- או |)
        setChar(originalSymbol);
    }
    else {
        // אם כבינו - הופכים לבלתי נראה (רווח)
        setChar(' ');
    }
}

bool Laser::handleCollision(Player& p, const Screen& screen)
{
    // אם הלייזר כבוי - הוא כמו אוויר, אפשר לעבור
    if (!active) {
        return true;
    }

    // אם הלייזר דולק - אי אפשר לעבור (או שהשחקן מת)
    return false;
}