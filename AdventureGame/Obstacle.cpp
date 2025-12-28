#include "Obstacle.h"
#include "Player.h"
#include "Screen.h"

Obstacle::Obstacle(const std::vector<Point>& tiles, ScreenId screenId)
    : GameObject(
        tiles.empty() ? 0 : tiles.front().getX(),
        tiles.empty() ? 0 : tiles.front().getY(),
        '*', screenId, true, false),
    parts(tiles)
{
}

bool Obstacle::isAtPosition(int x, int y) const
{
    for (const auto& part : parts) {
        if (part.getX() == x && part.getY() == y) return true;
    }
    return false;
}

void Obstacle::drawToBuffer(std::vector<std::string>& buffer) const
{
    for (const auto& part : parts) {
        int x = part.getX();
        int y = part.getY();
        if (y >= 0 && y < (int)buffer.size() && x >= 0 && x < (int)buffer[y].size()) {
            buffer[y][x] = part.getChar();
        }
    }
}

int Obstacle::computeForce(const Player& player) const
{
    return player.getForce();
}

void Obstacle::directionToDelta(Direction dir, int& dx, int& dy)
{
    dx = 0; dy = 0;
    switch (dir) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1;  break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1;  break;
    default: break;
    }
}

bool Obstacle::canMove(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const
{
    for (const auto& part : parts) {
        int nx = part.getX() + dx;
        int ny = part.getY() + dy;

        // Check walls
        if (screen.isWall(Point(nx, ny, ' '))) return false;

        // Check other objects
        for (const auto& obj : gameObjects) {
            if (obj == this) continue;
            if (obj->getScreenId() != screenID) continue;
            if (obj->isAtPosition(nx, ny) && obj->isSolid()) return false;
        }
    }
    return true;
}

void Obstacle::moveBy(int dx, int dy)
{
    for (auto& part : parts) {
        part.move(dx, dy);
    }
    // Update base position for consistency
    point.move(dx, dy);
}

bool Obstacle::isAssistingPush(const Player& other, int dx, int dy) const
{
    int pdx = 0, pdy = 0;
    directionToDelta(other.getDirection(), pdx, pdy);

    // Check if pushing in same direction
    if (pdx != dx || pdy != dy) return false;

    // Check adjacency
    int tx = other.getX() + pdx;
    int ty = other.getY() + pdy;
    return isAtPosition(tx, ty);
}

bool Obstacle::handleCollision(Player& p, const Screen& screen)
{
    return false; // Single player cannot push alone (unless force allows)
}

bool Obstacle::handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects)
{
    int dx = 0, dy = 0;
    directionToDelta(p.getDirection(), dx, dy);

    if (dx == 0 && dy == 0) return false;

    int totalForce = computeForce(p);
    if (otherPlayer && isAssistingPush(*otherPlayer, dx, dy)) {
        totalForce += computeForce(*otherPlayer);
    }

    int requiredForce = static_cast<int>(parts.size());

    if (totalForce < requiredForce) return false;

    if (!canMove(dx, dy, screen, gameObjects)) return false;

    moveBy(dx, dy);
    return true;
}

bool Obstacle::handleExplosionAt(int x, int y)
{
    for (auto it = parts.begin(); it != parts.end(); ++it)
    {
        if (it->getX() == x && it->getY() == y) {
            parts.erase(it);
            break;
        }
    }
    return parts.empty();
}