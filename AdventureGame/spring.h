#pragma once
#include "Point.h"
#include "ScreenID.h"
#include "Player.h"
#include "GameObject.h"


class Spring :public GameObject
{
private:
    Point start;
    Point middle;
    Point end;
	Direction direction;//to wich direction the spring will throw the player

    void loadSpring(Player& p);
public:
    // --- Constructor ---
    // startPt/midPt/endPt – הנקודות של הקפיץ על המסך
    Spring(const Point& startPt,
        const Point& middlePt,
        const Point& endPt,
        Direction dir,
        ScreenId screenId)
        // בנאי GameObject: מיקום "ראשי" של האובייקט = נקודת ההתחלה של הקפיץ
        // char – איך נרצה שייראה ה"סגמנט" (כרגע '#')
        // solid=false, collected=false – קפיץ לא חוסם כמו קיר ולא נאסף
        : GameObject(endPt.getX(), endPt.getY(), 'W', screenId, false, false),
        start(startPt),
        middle(middlePt),
        end(endPt),
        direction(dir)
    {}

    // --- Getters ---
    const Point& getStart()  const { return start; }
    const Point& getMiddle() const { return middle; }
    const Point& getEnd()    const { return end; }
    Direction getDirection() const { return direction; }
	int getLength() const { return 3; } // אורך הקפיץ בשלושת החלקים שלו

    // ציור של שלושת החלקים לבאפר (נשתמש בזה מתוך Game::drawObjectsToBuffer)
    void drawToBuffer(std::vector<std::string>& buffer) const;

    virtual bool isAtPosition(int x, int y) const override;
    virtual bool handleCollision(Player& p, const Screen& screen) override;
    static Direction oppositeDirection(Direction d);
};