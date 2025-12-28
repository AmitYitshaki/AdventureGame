#include "GameObject.h"
#include "Player.h"

void GameObject::dropToScreen(int x, int y)
{
    point.setPos(x, y);
    collected = false;
}

void GameObject::removeFromGame()
{
    setPosition(-1, -1);
}

void GameObject::drawToBuffer(std::vector<std::string>& buffer) const
{
    int x = point.getX();
    int y = point.getY();
    if (y >= 0 && y < (int)buffer.size() && x >= 0 && x < (int)buffer[y].size())
    {
        buffer[y][x] = point.getChar();
    }
}

bool GameObject::handleCollision(Player& p, const Screen& screen)
{
    if (!isSolid())
    {
        if (!isCollected()) p.collectItem(this);
        return true;
    }
    return false;
}

bool GameObject::handleExplosionAt(int x, int y)
{
    return isAtPosition(x, y);
}