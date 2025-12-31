#include "Spring.h"
#include "Player.h"
#include "Screen.h"

Direction Spring::oppositeDirection(Direction d)
{
    switch (d) {
    case Direction::UP:    return Direction::DOWN;
    case Direction::DOWN:  return Direction::UP;
    case Direction::LEFT:  return Direction::RIGHT;
    case Direction::RIGHT: return Direction::LEFT;
    default:               return Direction::STAY;
    }
}

void Spring::getDelta(Direction d, int& dx, int& dy)
{
    dx = 0; dy = 0;
    switch (d) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1;  break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1;  break;
    }
}

Spring::Spring(const Point& basePosition, Direction dir, ScreenId screenId, int length)
    : GameObject(basePosition.getX(), basePosition.getY(), 'W', screenId, false, false),
    direction(dir)
{
    rebuild(dir, length);
}

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    for (const auto& p : parts) {
        int x = p.getX();
        int y = p.getY();
        if (x >= 0 && y >= 0 && y < (int)buffer.size() && x < (int)buffer[y].size()) {
            buffer[y][x] = p.getChar();
        }
    }
}

bool Spring::isAtPosition(int x, int y) const
{
    for (const auto& p : parts) {
        if (p.getX() == x && p.getY() == y) return true;
    }
    return false;
}

void Spring::rebuild(Direction dir, int length)
{
    parts.clear();
    direction = dir;
    oppositeDir = oppositeDirection(dir);

    int startX = getX();
    int startY = getY();
    int dx, dy;
    getDelta(dir, dx, dy);

    for (int i = 0; i < length; ++i)
    {
        char c = (i == 0) ? 'W' : 'w';
        parts.emplace_back(startX + (i * dx), startY + (i * dy), c);
    }
}

void Spring::setDirection(Direction newDir)
{
    if (parts.empty()) return;
    direction = newDir;
    oppositeDir = oppositeDirection(newDir);

    int startX = parts[0].getX();
    int startY = parts[0].getY();
    int dx, dy;
    getDelta(direction, dx, dy);

    for (size_t i = 0; i < parts.size(); ++i) {
        parts[i].setPos(startX + (int)i * dx, startY + (int)i * dy);
    }
}

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    if (parts.empty()) return true;

    int px = p.getX();
    int py = p.getY();
    int hitIndex = -1;

    for (size_t i = 0; i < parts.size(); ++i) {
        if (parts[i].getX() == px && parts[i].getY() == py) {
            hitIndex = (int)i;
            break;
        }
    }

    if (hitIndex == -1) return true;

    if (p.isFlying()) {
        for (size_t i = 0; i < parts.size(); ++i) {
            char c = (i == 0) ? 'W' : 'w';
            parts[i].setChar(c);
        }
        return true;
    }

    Direction pDir = p.getDirection();
    if (pDir != oppositeDir && pDir != Direction::STAY) {
        return false;
    }

    parts[hitIndex].setChar('_');

    if (hitIndex == 0) {
        for (auto& pt : parts) pt.setChar('_');
        p.setLaunchDirection(direction);
        p.launch((int)parts.size());
        return true;
    }

    if (pDir == Direction::STAY) {
        p.setLaunchDirection(direction);
        int power = (int)parts.size() - hitIndex;
        if (power < 2) power = 2;
        p.launch(power);
        return true;
    }

    return true;
}

bool Spring::handleExplosionAt(int x, int y)
{
    if (parts.empty()) return true;

    for (auto it = parts.begin(); it != parts.end(); ++it)
    {
        if (it->getX() == x && it->getY() == y) {
            bool isBase = (it == parts.begin());
            parts.erase(it);
            if (isBase) return true;
            break;
        }
    }
    return parts.empty();
}