#include "Spring.h"
#include "Player.h"
#include "Screen.h"
#include <iostream> 

// =========================================================
//                   HELPER FUNCTIONS
// =========================================================

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

static void putPointToBuffer(std::vector<std::string>& buffer, const Point& pt)
{
    int x = pt.getX();
    int y = pt.getY();
    // הגנה מפני נקודות מחוקות או חריגה מהמסך
    if (x >= 0 && y >= 0 && y < (int)buffer.size() && x < (int)buffer[y].size()) {
        buffer[y][x] = pt.getChar();
    }
}

// =========================================================
//                   CONSTRUCTOR
// =========================================================

Spring::Spring(const Point& basePosition, Direction dir, ScreenId screenId, int length)
    : GameObject(basePosition.getX(), basePosition.getY(), 'W', screenId, false, false),
    direction(dir)
{
    // הבנאי משתמש ב-rebuild כדי ליצור את החלקים בפעם הראשונה
    rebuild(dir, length);
}

// =========================================================
//                     DRAWING
// =========================================================

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    for (const auto& p : parts) {
        putPointToBuffer(buffer, p);
    }
}

// =========================================================
//                POSITION & SETUP
// =========================================================

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

    // אנחנו מניחים שהמיקום של האובייקט עצמו (point ב-GameObject) הוא הבסיס
    int startX = getX();
    int startY = getY();

    int dx, dy;
    getDelta(dir, dx, dy);

    for (int i = 0; i < length; ++i)
    {
        // החלק הראשון (0) הוא הבסיס 'W', השאר 'w'
        char c = (i == 0) ? 'W' : 'w';
        parts.emplace_back(startX + (i * dx), startY + (i * dy), c);
    }
}

void Spring::setDirection(Direction newDir)
{
    // אם משנים כיוון באמצע משחק, מסובבים את החלקים הקיימים ביחס לבסיס
    if (parts.empty()) return;

    direction = newDir;
    oppositeDir = oppositeDirection(newDir);

    Point base = parts[0];
    int startX = base.getX();
    int startY = base.getY();

    int dx, dy;
    getDelta(direction, dx, dy);

    for (size_t i = 0; i < parts.size(); ++i)
    {
        // עדכון המיקום החדש לפי האינדקס היחסי
        parts[i].setPos(startX + (int)i * dx, startY + (int)i * dy);
    }
}

// =========================================================
//                 COLLISION LOGIC
// =========================================================

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    if (parts.empty()) return true;

    int px = p.getX();
    int py = p.getY();

    // 1. בדיקה: האם השחקן עומד על אחד מחלקי הקפיץ?
    int hitIndex = -1;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (parts[i].getX() == px && parts[i].getY() == py) {
            hitIndex = (int)i;
            break;
        }
    }

    // השחקן לא נוגע בקפיץ
    if (hitIndex == -1) return true;

    // --- מצב יציאה (Flying) ---
    // השחקן עף מעל הקפיץ - רק ויזואליזציה (Reset למראה מקורי)
    if (p.isFlying())
    {
        for (size_t i = 0; i < parts.size(); ++i) {
            char c = (i == 0) ? 'W' : 'w';
            parts[i].setChar(c);
        }
        return true;
    }

    // --- מצב כניסה (Walking) ---

    // חסימה: אפשר להיכנס רק מהכיוון ההפוך (או לעמוד עליו ב-STAY)
    if (p.getDirection() != oppositeDir && p.getDirection() != Direction::STAY) {
        return false; // חסימה פיזית
    }

    // שינוי ויזואלי - "דריכה"
    parts[hitIndex].setChar('_');

    // אם דורך על הבסיס (אינדקס 0) - טעינה!
    if (hitIndex == 0)
    {
        // משטחים את כל הקפיץ ויזואלית
        for (auto& pt : parts) pt.setChar('_');
        loadSpring(p);
    }

    return true;
}

void Spring::loadSpring(Player& p)
{
    p.stopMovement();
    p.setLoaded(true);
    p.setLaunchDirection(direction);

    // הכוח נקבע לפי כמות החלקים שנותרו בקפיץ
    p.setLoadedSpringLen((int)parts.size());
}

// =========================================================
//                 EXPLOSION HANDLING
// =========================================================

bool Spring::handleExplosionAt(int x, int y)
{
    if (parts.empty()) return true;

    for (auto it = parts.begin(); it != parts.end(); ++it)
    {
        if (it->getX() == x && it->getY() == y)
        {
            // בדיקה האם זה הבסיס (האלמנט הראשון בווקטור)
            bool isBase = (it == parts.begin());

            // מחיקת החלק מהווקטור
            parts.erase(it);

            // אם הבסיס הלך - כל הקפיץ מושמד
            if (isBase) return true;

            // יצאנו כי מחקנו איבר והאיטרטור כבר לא תקף
            break;
        }
    }

    // אם לא נשארו חלקים - למחוק את האובייקט
    return parts.empty();
}