#include "Obstacle.h"
#include "Laser.h"
#include "Switch.h"
#include "Screen.h"
#include "Player.h"
#include "Game.h"
#include <iostream>



Obstacle::Obstacle(const std::vector<Point>& tiles, ScreenId screenId)
    : GameObject(tiles.empty() ? 0 : tiles.front().getX(), tiles.empty() ? 0 : tiles.front().getY(), '*', screenId, true, false), parts(tiles) {
}

bool Obstacle::isAtPosition(int x, int y) const {
    for (const auto& part : parts) if (part.getX() == x && part.getY() == y) return true;
    return false;
}

void Obstacle::drawToBuffer(std::vector<std::string>& buffer) const {
    for (const auto& part : parts) {
        int x = part.getX(); int y = part.getY();
        if (y >= 0 && y < (int)buffer.size() && x >= 0 && x < (int)buffer[y].size()) buffer[y][x] = part.getChar();
    }
}

bool Obstacle::handleExplosionAt(int x, int y) {
    for (auto it = parts.begin(); it != parts.end(); ++it) {
        if (it->getX() == x && it->getY() == y) { parts.erase(it); break; }
    }
    return parts.empty();
}

void Obstacle::getDirectionDelta(Direction dir, int& dx, int& dy) {
    dx = 0; dy = 0;
    switch (dir) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1;  break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1;  break;
    default: break;
    }
}

// === התיקון הקריטי נמצא כאן ===
int Obstacle::getAssistForce(const Player& helper, const Player& primaryPusher, int pushDx, int pushDy) const
{
    // 1. כיוון: חייבים לדחוף לאותו כיוון
    int helperDx = 0, helperDy = 0;
    getDirectionDelta(helper.getDirection(), helperDx, helperDy);

    if (helperDx != pushDx || helperDy != pushDy) return 0;

    // מיקום היעד של העוזר (לאן הוא רוצה להגיע)
    int helperTargetX = helper.getX() + helperDx;
    int helperTargetY = helper.getY() + helperDy;

    // בדיקה 1: Side-by-Side (העוזר נוגע ישירות במכשול)
    if (isAtPosition(helperTargetX, helperTargetY)) {
        return helper.getForce();
    }

    // בדיקה 2: Train Push (P2 -> P1 -> Obstacle)
    // בגלל ש-P1 כבר זז לוגית (נמצא בתוך המכשול), אנחנו צריכים לבדוק שתי אפשרויות:

    // אופציה א': העוזר דוחף למקום שבו P1 נמצא כרגע (במקרה ש-P1 עוד לא זז בזיכרון)
    if (helperTargetX == primaryPusher.getX() && helperTargetY == primaryPusher.getY()) {
        return helper.getForce();
    }

    // אופציה ב' (התיקון): העוזר דוחף למקום ש-P1 *היה* בו לפני רגע.
    // מכיוון ש-P1 זז בכיוון הדחיפה, המיקום הקודם שלו הוא (X - dx, Y - dy)
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
            if (obj == this) continue;

            if (obj->getScreenId() != this->getScreenId()) continue;

            if (!obj->isAtPosition(nextX, nextY)) continue;

            if (auto laser = dynamic_cast<Laser*>(obj)) {
                if (laser->isActive()) return false;
                continue;
            }
            return false;
        }
    }
    return true;
}

void Obstacle::applyMovement(int dx, int dy) {
    for (auto& part : parts) part.move(dx, dy);
    point.move(dx, dy);
}

bool Obstacle::handleCollision(Player& p, const Screen& screen, const Player* otherPlayer, const std::vector<GameObject*>& gameObjects)
{
    Direction dir = p.isFlying() ? p.getLaunchDirection() : p.getDirection();
    int dx = 0, dy = 0;
    getDirectionDelta(dir, dx, dy);

    if (dx == 0 && dy == 0) return false;

    int totalForce = p.getForce();
    if (otherPlayer) {
        totalForce += getAssistForce(*otherPlayer, p, dx, dy);
    }

    int obstacleMass = static_cast<int>(parts.size());

    // דיבאג קטן שאפשר להשאיר או למחוק
    // אם המשקל גדול מהכוח - נכשל
    if (totalForce < obstacleMass) return false;

    if (!isPathClear(dx, dy, screen, gameObjects)) return false;

    applyMovement(dx, dy);
    return true;
}

bool Obstacle::handleCollision(Player& p, const Screen& screen) { return false; }

void Obstacle::printDebugInfo() const {
    std::cout << "[Obstacle @" << this << "] Parts: ";
    for (const auto& p : parts) {
        std::cout << "(" << p.getX() << "," << p.getY() << ") ";
    }
    std::cout << std::endl;
}