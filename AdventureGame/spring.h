#pragma once

#include <vector>
#include <string>
#include "Point.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Direction.h"

// Forward declarations
class Player;
class Screen;

/*
    Spring:
    A dynamic multi-segment spring that compresses when a player
    moves over it from the correct direction.
    Supports dynamic length and partial destruction by explosions.
*/
class Spring : public GameObject
{
public:
    // בנאי: מקבל מיקום בסיס, כיוון, מסך ואורך התחלתי
    Spring(const Point& basePosition,
        Direction dir,
        ScreenId screenId,
        int length);

    // --- Drawing ---
	void drawToBuffer(std::vector<std::string>& buffer) const override;

    // --- Collision & Logic ---
    bool isAtPosition(int x, int y) const override;
    bool handleCollision(Player& p, const Screen& screen) override;

    // --- Explosion Handling ---
    // מוחקת חלק מהקפיץ שנפגע. 
    // מחזירה true אם הקפיץ הושמד כליל (אם הבסיס נפגע או לא נשארו חלקים)
	bool handleExplosionAt(int x, int y) override;

    // --- Properties ---
    void setDirection(Direction newDir); // מעדכנת כיוון לכל החלקים הקיימים
    Direction getDirection() const { return direction; }
    Direction getOppositeDirection() const { return oppositeDir; }

    // הכוח שווה למספר החלקים שנותרו כרגע
    int getLength() const { return (int)parts.size(); }

    // פונקציה לבניית הקפיץ מחדש (נקראת ע"י LevelLoader לטעינת נתונים)
    void rebuild(Direction dir, int length);

private:
    // parts[0] תמיד יהיה הבסיס (החלק שטוען את הקפיץ)
    std::vector<Point> parts;

    Direction direction;
    Direction oppositeDir;

    void loadSpring(Player& p);

    // פונקציות עזר פנימיות
    static void getDelta(Direction d, int& dx, int& dy);
    static Direction oppositeDirection(Direction d);
};