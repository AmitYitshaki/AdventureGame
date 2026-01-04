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
    // Format: X Y OriginalSymbol
    // Note: Active state depends on Switch, but we save original char to restore correctly
    std::string s = GameObject::getSaveData();
    s += " ";
    s += originalSymbol;
    return s;
}