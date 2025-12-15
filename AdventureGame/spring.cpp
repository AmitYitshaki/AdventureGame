#include "Spring.h"
#include "Player.h"
#include "Screen.h"
#include "Game.h"
#include <vector>

/*
    Spring.cpp
    Handles spring drawing, collision logic, compression animation,
    and triggering of the player spring-loading behavior.
*/

// Helper: write single spring segment to output buffer
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

/* ----------------------------------------------------
                        DRAWING
   ---------------------------------------------------- */

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    putPointToBuffer(buffer, start);
    putPointToBuffer(buffer, middle);
    putPointToBuffer(buffer, end);
}

/* ----------------------------------------------------
                  POSITION & COLLISION
   ---------------------------------------------------- */

bool Spring::isAtPosition(int x, int y) const
{
    return  (x == start.getX() && y == start.getY()) ||
        (x == middle.getX() && y == middle.getY()) ||
        (x == end.getX() && y == end.getY());
}

Direction Spring::oppositeDirection(Direction d)
{
    switch (d)
    {
    case Direction::UP:    return Direction::DOWN;
    case Direction::DOWN:  return Direction::UP;
    case Direction::LEFT:  return Direction::RIGHT;
    case Direction::RIGHT: return Direction::LEFT;
    default:               return d;
    }
}

// שומרים על בניית הקפיץ "קדימה" כדי שהציור יתאים
void Spring::setDirection(Direction newDir)
{
    direction = newDir;
    int x = start.getX();
    int y = start.getY();

    switch (direction)
    {
    case Direction::UP:    // Body expands Down (launch up)
        middle.setPos(x, y - 1); // תיקון: אם משגרים למעלה, הגוף צריך להיות למעלה (ויזואלית) או למטה?
        // בגרסה האחרונה שעבדה ויזואלית השתמשנו בלוגיקה של הכיוון עצמו:
        // UP = Y קטן.
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

/* ----------------------------------------------------
                SPRING COLLISION LOGIC
   ---------------------------------------------------- */

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    int px = p.getX();
    int py = p.getY();

    bool onStart = (px == start.getX() && py == start.getY());   // המיקום של ה-W
    bool onMiddle = (px == middle.getX() && py == middle.getY()); // המיקום של ה-w האמצעי
    bool onEnd = (px == end.getX() && py == end.getY());         // המיקום של ה-w בקצה

    if (!onStart && !onMiddle && !onEnd)
        return true;

    // =========================================================
    //                מצב תעופה (Flying - יציאה)
    // =========================================================
    // כשהשחקן משוגר, הוא עף מה-Start לכיוון ה-End.
    // אנחנו ננצל את המעבר הזה כדי "לאפס" את המקפצה מאחוריו.
    if (p.isFlying())
    {
        if (onStart) {
            start.setChar('w'); // נראה מכווץ כשאנחנו עליו
        }
        else if (onMiddle) {
            middle.setChar('w');
            start.setChar('W');  // עזבנו את ה-Start, אז הוא חוזר להיות W גדול
        }
        else if (onEnd) {
            end.setChar('w');
            middle.setChar('w'); // עזבנו את האמצע, הוא חוזר להיות w קטן
            start.setChar('W');  // ליתר ביטחון
        }
        return true;
    }

    // =========================================================
    //                מצב הליכה (Walking - כניסה)
    // =========================================================

    // 1. בדיקת כיוון: חובה לבוא *נגד* הכיוון של הקפיץ כדי לדרוך אותו.
    // אם הקפיץ יורה ימינה, צריך ללכת שמאלה כדי להיכנס אליו.
    Direction neededDir = Spring::oppositeDirection(direction);

    if (p.getDirection() != neededDir)
    {
        return false; // חסימה! (מתנהג כמו קיר אם באים מהצד או מהכיוון הלא נכון)
    }

    // 2. כניסה לקפיץ (מהקצה לבסיס)

    if (onEnd) {
        end.setChar('_'); // אנימציית כיווץ של הקצה
        return true;      // מאפשרים לעבור
    }

    if (onMiddle) {
        middle.setChar('_'); // אנימציית כיווץ של האמצע
        return true;         // מאפשרים לעבור
    }

    // 3. הגעה לבסיס (Start / W) - כאן העצירה!
    if (onStart)
    {
        // כיווץ ויזואלי של כל הקפיץ
        start.setChar('_');
        middle.setChar('_');
        end.setChar('_');

        // עצירה וטעינה
        loadSpring(p);
        return true;
    }

    return true;
}
/* ----------------------------------------------------
                LOAD PLAYER FOR LAUNCH
   ---------------------------------------------------- */

void Spring::loadSpring(Player& p)
{
    p.stopMovement();
    p.setLoaded(true);
    p.setLaunchDirection(direction);
    p.setLoadedSpringLen(getLength());
}