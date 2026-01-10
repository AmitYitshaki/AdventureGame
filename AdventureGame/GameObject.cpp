#include "GameObject.h"
#include "Player.h"
#include "GameException.h"

// === Inventory & Lifecycle ===
void GameObject::dropToScreen(int x, int y)
{
    if (x != -1) {
        GameException::ensureInBounds(x, y, "GameObject::dropToScreen");
    }
    point.setPos(x, y);
    collected = false;
}

void GameObject::removeFromGame()
{
    setPosition(-1, -1);
}

// === Rendering ===
void GameObject::drawToBuffer(std::vector<std::string>& buffer) const
{
    int x = point.getX();
    int y = point.getY();
    if (y >= 0 && y < static_cast<int>(buffer.size()) && x >= 0 && x < static_cast<int>(buffer[y].size()))
    {
        buffer[y][x] = point.getChar();
    }
}

// === Interactions ===
bool GameObject::handleCollision(Player& p, const Screen& screen)
{
    if (!isSolid())
    {
        if (!isCollected()) {
            p.collectItem(this);
        }
        return true;
    }
    return false;
}

bool GameObject::handleExplosionAt(int x, int y)
{
    return isAtPosition(x, y);
}

// === Serialization ===
std::string GameObject::getSaveData() const
{
    return std::to_string(getX()) + " " + std::to_string(getY());
}
