#include "Obstacle.h"
#include "Player.h"
#include "Screen.h"

Obstacle::Obstacle(const std::vector<Point>& tiles, ScreenId screenId)
    : GameObject(
        tiles.empty() ? 0 : tiles.front().getX(),
        tiles.empty() ? 0 : tiles.front().getY(),
        '*',
        screenId,
        true,
        false),
    parts(tiles)
{
}

bool Obstacle::isAtPosition(int x, int y) const
{
    for (const auto& part : parts)
    {
        if (part.getX() == x && part.getY() == y)
            return true;
    }
    return false;
}

void Obstacle::drawToBuffer(std::vector<std::string>& buffer) const
{
    for (const auto& part : parts)
    {
        int x = part.getX();
        int y = part.getY();

        if (y >= 0 && y < (int)buffer.size() &&
            x >= 0 && x < (int)buffer[y].size())
        {
            buffer[y][x] = part.getChar();
        }
    }
}

int Obstacle::computeForce(const Player& player)
{
    return player.getForce();
}

void Obstacle::directionToDelta(Direction dir, int& dx, int& dy)
{
    dx = 0;
    dy = 0;
    switch (dir)
    {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1;  break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1;  break;
    case Direction::STAY:  break;
    }
}

bool Obstacle::canMove(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const
{
    for (const auto& part : parts)
    {
        int nx = part.getX() + dx;
        int ny = part.getY() + dy;

        if (nx < 0 || nx >= Screen::WIDTH || ny < 0 || ny >= Screen::HEIGHT)
            return false;

        if (screen.isWall(Point(nx, ny, ' ')))
            return false;

        for (const auto obj : gameObjects)
        {
            if (!obj || obj->isCollected())
                continue;

            if (obj->getScreenId() != screen.getScreenId())
                continue;

            if (obj != this && obj->isSolid() && obj->isAtPosition(nx, ny))
                return false;
        }
    }
    return true;
}

void Obstacle::moveBy(int dx, int dy)
{
    for (auto& part : parts)
    {
        part.move(dx, dy);
    }

    if (!parts.empty())
    {
        setPosition(parts.front().getX(), parts.front().getY());
    }
}

bool Obstacle::isAssistingPush(const Player& other, int dx, int dy) const
{
    int otherDx = 0, otherDy = 0;
    directionToDelta(other.getDirection(), otherDx, otherDy);
    if (otherDx != dx || otherDy != dy)
        return false;

    for (const auto& part : parts)
    {
        if (other.getX() == part.getX() - dx &&
            other.getY() == part.getY() - dy)
        {
            return true;
        }
    }

    return false;
}

bool Obstacle::handleCollision(Player& p, const Screen& screen)
{
    static const std::vector<GameObject*> empty;
    return handleCollision(p, screen, nullptr, empty);
}

bool Obstacle::handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects)
{
    int dx = 0, dy = 0;
    directionToDelta(p.getDirection(), dx, dy);

    if (dx == 0 && dy == 0)
        return false;

    int totalForce = computeForce(p);
    if (otherPlayer && isAssistingPush(*otherPlayer, dx, dy))
    {
        totalForce += computeForce(*otherPlayer);
    }

    int requiredForce = static_cast<int>(parts.size());

    if (totalForce < requiredForce)
        return false;

    if (!canMove(dx, dy, screen, gameObjects))
        return false;

    moveBy(dx, dy);
    return true;
}

// =========================================================
//            EXPLOSION HANDLING (התוספת)
// =========================================================

bool Obstacle::handleExplosionAt(int x, int y)
{
    // משתמשים ב-parts (כמו בקוד המקורי שלך)
    for (auto it = parts.begin(); it != parts.end(); ++it)
    {
        if (it->getX() == x && it->getY() == y)
        {
            parts.erase(it);

            // עדכון המיקום הראשי של האובייקט אם החלק הראשון נמחק
            // (חשוב כדי שפונקציות כמו getX יעבדו נכון)
            if (!parts.empty()) {
                setPosition(parts.front().getX(), parts.front().getY());
            }

            break; // מצאנו ומחקנו
        }
    }

    // החזרה: האם המכשול ריק? (אם כן, ה-Game ימחק אותו לגמרי)
    return parts.empty();
}