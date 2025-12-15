#include "Switch.h"

Switch::Switch(int x, int y, ScreenId screen)
    : GameObject(x, y, '/', screen, false), // solid = false (אפשר לדרוך)
    active(false)
{
}

void Switch::addTarget(GameObject* obj)
{
    if (obj != nullptr) {
        targets.push_back(obj);
    }
}

bool Switch::handleCollision(Player& p, const Screen& screen)
{
    // ברגע שדורכים - משנים מצב
    toggle();
    return true; // מאפשר לשחקן לעמוד על המתג
}

void Switch::toggle()
{
    // הופך את המצב (מ-false ל-true ולהפך)
    active = !active;

    // שינוי גרפי של המתג עצמו (אופציונלי: / או \)
    // שים לב: '\\' זה הסימון ל-Backslash בודד
    char visual = active ? '\\' : '/';
    setChar(visual);

    // שליחת העדכון לכל הלייזרים המחוברים
    for (GameObject* target : targets)
    {
        target->receiveSignal(active);
    }
}