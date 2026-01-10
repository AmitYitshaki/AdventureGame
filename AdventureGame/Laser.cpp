#include "Laser.h"

// === Construction ===
Laser::Laser(int x, int y, char c, ScreenId screen)
    : GameObject(x, y, c, screen, true),
    active(true),
    originalSymbol(c)
{
}

// === Switch Signaling ===
void Laser::receiveSignal(bool switchState)
{
    // Active if switch is OFF. Inactive if switch is ON.
    active = !switchState;
    if (active) {
        setChar(originalSymbol);
    }
    else {
        setChar(' ');
    }
}

// === Collision Handling ===
bool Laser::handleCollision(Player& p, const Screen& screen)
{
    return !active; // Passable only if inactive.
}

// === Serialization ===
std::string Laser::getTypeName() const { return "LASER"; }

std::string Laser::getSaveData() const
{
    // Format: X Y OriginalSymbol Active(1/0)
    std::string s = GameObject::getSaveData();
    s += " ";
    s += originalSymbol;
    s += " ";
    s += (active ? "1" : "0"); // Persist the current active state.
    return s;
}
