#include "Obstacle.h"
#include "Laser.h"
#include "Switch.h"
#include "Screen.h"
#include "Player.h"
#include "Game.h"
#include <iostream>

// === Construction ===
Obstacle::Obstacle(const std::vector<Point>& tiles, ScreenId screenId)
    : GameObject(tiles.empty() ? 0 : tiles.front().getX(),
        tiles.empty() ? 0 : tiles.front().getY(),
        '*',
        screenId,
        true,
        false),
    parts(tiles)
{
}

// === Queries ===
bool Obstacle::isAtPosition(int x, int y) const
{
    for (const auto& part : parts) {
        if (part.getX() == x && part.getY() == y) {
            return true;
        }
    }
    return false;
}

// === Rendering ===
void Obstacle::drawToBuffer(std::vector<std::string>& buffer) const
{
    for (const auto& part : parts) {
        int x = part.getX();
        int y = part.getY();
        if (y >= 0 && y < static_cast<int>(buffer.size())
            && x >= 0 && x < static_cast<int>(buffer[y].size())) {
            buffer[y][x] = part.getChar();
        }
    }
}

// === Explosions ===
bool Obstacle::handleExplosionAt(int x, int y)
{
    for (auto it = parts.begin(); it != parts.end(); ++it) {
        if (it->getX() == x && it->getY() == y) {
            parts.erase(it);
            break;
        }
    }
    return parts.empty();
}

// === Movement Helpers ===
void Obstacle::getDirectionDelta(Direction dir, int& dx, int& dy)
{
    dx = 0;
    dy = 0;
    switch (dir) {
    case Direction::UP:
        dy = -1;
        break;
    case Direction::DOWN:
        dy = 1;
        break;
    case Direction::LEFT:
        dx = -1;
        break;
    case Direction::RIGHT:
        dx = 1;
        break;
    default:
        break;
    }
}

int Obstacle::getAssistForce(const Player& helper, const Player& primaryPusher, int pushDx, int pushDy) const
{
    // The helper must push in the same direction as the primary pusher.
    int helperDx = 0;
    int helperDy = 0;
    getDirectionDelta(helper.getDirection(), helperDx, helperDy);

    if (helperDx != pushDx || helperDy != pushDy) {
        return 0;
    }

    // The helper's target location (where they want to move next).
    int helperTargetX = helper.getX() + helperDx;
    int helperTargetY = helper.getY() + helperDy;

    // Case 1: Side-by-side push (helper directly touches the obstacle).
    if (isAtPosition(helperTargetX, helperTargetY)) {
        return helper.getForce();
    }

    // Case 2: Train push (P2 -> P1 -> Obstacle).
    // We need to consider both the current and previous position of P1.

    // Option A: Helper pushes into the primary pusher's current position.
    if (helperTargetX == primaryPusher.getX() && helperTargetY == primaryPusher.getY()) {
        return helper.getForce();
    }

    // Option B: Helper pushes into the primary pusher's previous position.
    // Because P1 already moved in the push direction, their previous position
    // is (X - dx, Y - dy).
    int p1PreviousX = primaryPusher.getX() - pushDx;
    int p1PreviousY = primaryPusher.getY() - pushDy;

    if (helperTargetX == p1PreviousX && helperTargetY == p1PreviousY) {
        return helper.getForce();
    }

    return 0;
}

bool Obstacle::isPathClear(int dx, int dy, const Screen& screen, const std::vector<GameObject*>& gameObjects) const
{
    for (const auto& part : parts) {
        int nextX = part.getX() + dx;
        int nextY = part.getY() + dy;

        if (!screen.inBounds(nextX, nextY) || screen.isWall(Point(nextX, nextY, ' '))) {
            return false;
        }

        for (const auto& obj : gameObjects) {
            if (obj == this) {
                continue;
            }

            if (obj->getScreenId() != this->getScreenId()) {
                continue;
            }

            if (!obj->isAtPosition(nextX, nextY)) {
                continue;
            }

            if (auto laser = dynamic_cast<Laser*>(obj)) {
                if (laser->isActive()) {
                    return false;
                }
                continue;
            }
            return false;
        }
    }
    return true;
}

void Obstacle::applyMovement(int dx, int dy)
{
    for (auto& part : parts) {
        part.move(dx, dy);
    }
    point.move(dx, dy);
}

// === Collision Handling ===
bool Obstacle::handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects)
{
    Direction dir = p.isFlying() ? p.getLaunchDirection() : p.getDirection();
    int dx = 0;
    int dy = 0;
    getDirectionDelta(dir, dx, dy);

    if (dx == 0 && dy == 0) {
        return false;
    }

    int totalForce = p.getForce();
    if (otherPlayer) {
        totalForce += getAssistForce(*otherPlayer, p, dx, dy);
    }

    int obstacleMass = static_cast<int>(parts.size());

    // If the obstacle is heavier than the combined force, the push fails.
    if (totalForce < obstacleMass) {
        return false;
    }

    if (!isPathClear(dx, dy, screen, gameObjects)) {
        return false;
    }

    applyMovement(dx, dy);
    return true;
}

bool Obstacle::handleCollision(Player& p, const Screen& screen)
{
    return false;
}

// === Debugging ===
void Obstacle::printDebugInfo() const
{
    std::cout << "[Obstacle @" << this << "] Parts: ";
    for (const auto& p : parts) {
        std::cout << "(" << p.getX() << "," << p.getY() << ") ";
    }
    std::cout << std::endl;
}

// === Serialization ===
std::string Obstacle::getTypeName() const { return "OBSTACLE"; }

std::string Obstacle::getSaveData() const
{
    // Format: X Y Count Part1X Part1Y Part2X Part2Y ...
    std::string s = GameObject::getSaveData() + " " + std::to_string(parts.size());
    for (const auto& p : parts) {
        s += " " + std::to_string(p.getX()) + " " + std::to_string(p.getY());
    }
    return s;
}
