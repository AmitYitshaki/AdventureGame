#include "Spring.h"
#include "Player.h"
#include "Screen.h"
#include "Game.h"
#include <vector>

/*
    Spring.cpp
    Handles spring logic and rendering.
*/

// =========================================================
//                   HELPER FUNCTIONS
// =========================================================

// Static helper to calculate opposite direction
Direction Spring::oppositeDirection(Direction d)
{
    switch (d)
    {
    case Direction::UP:    return Direction::DOWN;
    case Direction::DOWN:  return Direction::UP;
    case Direction::LEFT:  return Direction::RIGHT;
    case Direction::RIGHT: return Direction::LEFT;
    default:               return Direction::STAY; // Should not happen for spring
    }
}

static void putPointToBuffer(std::vector<std::string>& buffer, const Point& pt)
{
    int x = pt.getX();
    int y = pt.getY();

    if (y >= 0 && y < (int)buffer.size() &&
        x >= 0 && x < (int)buffer[y].size())
    {
        buffer[y][x] = pt.getChar();
    }
}

// =========================================================
//                   CONSTRUCTOR
// =========================================================

Spring::Spring(const Point& startPt, const Point& middlePt, const Point& endPt, Direction dir, ScreenId screenId)
    : GameObject(endPt.getX(), endPt.getY(), 'W', screenId, false, false),
    start(startPt),
    middle(middlePt),
    end(endPt),
    direction(dir)
{
    // חישוב ושמירה של הכיוון ההפוך כבר ביצירה
    this->oppositeDir = oppositeDirection(dir);
}

// =========================================================
//                     DRAWING
// =========================================================

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    putPointToBuffer(buffer, start);
    putPointToBuffer(buffer, middle);
    putPointToBuffer(buffer, end);
}

// =========================================================
//                POSITION & SETUP
// =========================================================

bool Spring::isAtPosition(int x, int y) const
{
    return (x == start.getX() && y == start.getY()) ||
        (x == middle.getX() && y == middle.getY()) ||
        (x == end.getX() && y == end.getY());
}

void Spring::setDirection(Direction newDir)
{
    direction = newDir;

    // עדכון המטמון של הכיוון ההפוך
    oppositeDir = oppositeDirection(newDir);

    int x = start.getX();
    int y = start.getY();

    // סידור הנקודות לפי הכיוון החדש
    switch (direction)
    {
    case Direction::UP:
        middle.setPos(x, y - 1);
        end.setPos(x, y - 2);
        break;
    case Direction::DOWN:
        middle.setPos(x, y + 1);
        end.setPos(x, y + 2);
        break;
    case Direction::LEFT:
        middle.setPos(x - 1, y);
        end.setPos(x - 2, y);
        break;
    case Direction::RIGHT:
        middle.setPos(x + 1, y);
        end.setPos(x + 2, y);
        break;
    case Direction::STAY:
        middle.setPos(x, y);
        end.setPos(x, y);
        break;
    }
}

// =========================================================
//                 COLLISION LOGIC
// =========================================================

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    int px = p.getX();
    int py = p.getY();

    bool onStart = (px == start.getX() && py == start.getY());
    bool onMiddle = (px == middle.getX() && py == middle.getY());
    bool onEnd = (px == end.getX() && py == end.getY());

    if (!onStart && !onMiddle && !onEnd)
        return true;

    // --- מצב יציאה (Flying) ---
    if (p.isFlying())
    {
        if (onStart) {
            start.setChar('w');
        }
        else if (onMiddle) {
            middle.setChar('w');
            start.setChar('W');
        }
        else if (onEnd) {
            end.setChar('w');
            middle.setChar('w');
            start.setChar('W');
        }
        return true;
    }

    // --- מצב כניסה (Walking) ---

    // 1. שימוש במשתנה השמור (Optimization)
    // אם כיוון השחקן שונה מהכיוון ההפוך של הקפיץ -> חסימה
    if (p.getDirection() != this->oppositeDir)
    {
        return false;
    }

    // 2. כניסה לקפיץ 
    if (onEnd) {
        end.setChar('_');
        return true;
    }

    if (onMiddle) {
        middle.setChar('_');
        return true;
    }

    // 3. הגעה לבסיס - טעינה
    if (onStart)
    {
        start.setChar('_');
        middle.setChar('_');
        end.setChar('_');

        loadSpring(p);
        return true;
    }

    return true;
}

// =========================================================
//                    LOADING LOGIC
// =========================================================

void Spring::loadSpring(Player& p)
{
    p.stopMovement();
    p.setLoaded(true);
    p.setLaunchDirection(direction);
    p.setLoadedSpringLen(getLength());
}