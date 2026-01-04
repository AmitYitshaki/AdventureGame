#include "Laser.h"

Laser::Laser(int x, int y, char c, ScreenId screen)
    : GameObject(x, y, c, screen, true),
    active(true),
    originalSymbol(c)
{
}

void Laser::receiveSignal(bool switchState)
{
    // Active if switch is OFF. Inactive if switch is ON.
    active = !switchState;
    if (active) setChar(originalSymbol);
    else setChar(' ');
}

bool Laser::handleCollision(Player& p, const Screen& screen)
{
    return !active; // Passable only if inactive
}

std::string Laser::getTypeName() const { return "LASER"; }

std::string Laser::getSaveData() const {
    // Format: X Y OriginalSymbol Active(1/0)
    std::string s = GameObject::getSaveData();
    s += " ";
    s += originalSymbol;
    s += " ";
    s += (active ? "1" : "0"); // שומרים גם את המצב הנוכחי!
    return s;
}