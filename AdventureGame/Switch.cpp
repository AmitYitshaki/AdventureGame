#include "Switch.h"

// === Construction ===
Switch::Switch(int x, int y, ScreenId screen)
    : GameObject(x, y, '/', screen, false), active(false)
{
}

// === Target Management ===
void Switch::addTarget(GameObject* obj)
{
    if (obj != nullptr) {
        targets.push_back(obj);
    }
}

// === Collision Handling ===
bool Switch::handleCollision(Player& p, const Screen& screen)
{
    toggle();
    return true;
}

// === State Changes ===
void Switch::toggle()
{
    active = !active;
    char visual = active ? '\\' : '/';
    setChar(visual);

    for (GameObject* target : targets) {
        target->receiveSignal(active);
    }
}

// === Serialization ===
std::string Switch::getTypeName() const { return "SWITCH"; }

std::string Switch::getSaveData() const
{
    // Format: X Y Active(1/0) Count [Tx1 Ty1 Tx2 Ty2 ...]
    std::string s = GameObject::getSaveData() + " " + (active ? "1" : "0");

    s += " " + std::to_string(targets.size());
    for (auto target : targets) {
        // Save the position of each laser linked to this switch.
        s += " " + std::to_string(target->getX()) + " " + std::to_string(target->getY());
    }
    return s;
}
