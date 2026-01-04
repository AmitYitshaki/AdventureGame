#include "Switch.h"

Switch::Switch(int x, int y, ScreenId screen)
    : GameObject(x, y, '/', screen, false), active(false)
{
}

void Switch::addTarget(GameObject* obj)
{
    if (obj != nullptr) targets.push_back(obj);
}

bool Switch::handleCollision(Player& p, const Screen& screen)
{
    toggle();
    return true;
}

void Switch::toggle()
{
    active = !active;
    char visual = active ? '\\' : '/';
    setChar(visual);

    for (GameObject* target : targets) {
        target->receiveSignal(active);
    }
}

std::string Switch::getTypeName() const { return "SWITCH"; }
std::string Switch::getSaveData() const {
    // Format: X Y Active(1/0)
    return GameObject::getSaveData() + " " + (active ? "1" : "0");
}