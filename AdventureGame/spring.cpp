#include "spring.h"
#include "Screen.h"
#include "Game.h"
#include "Direction.h"

// עוזר קטן לכתיבה לבאפר
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

void Spring::drawToBuffer(std::vector<std::string>& buffer) const
{
    putPointToBuffer(buffer, start);
    putPointToBuffer(buffer, middle);
    putPointToBuffer(buffer, end);
}

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
    default:               return d; // or Direction::STAY
    }
}

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    int px = p.getX();
    int py = p.getY();

    // 1) We only care if player is actually on one of the spring cells
    bool onStart = (px == start.getX() && py == start.getY());
    bool onMiddle = (px == middle.getX() && py == middle.getY());
    bool onEnd = (px == end.getX() && py == end.getY());

    if (!onStart && !onMiddle && !onEnd) {
        // shouldn't really happen אם isAtPosition עובד, אבל ליתר ביטחון
        return true;
    }

    // 2) Check if the player is moving from the "correct" side:
    //    must come from the opposite direction of the spring.
    Direction neededDir = Spring::oppositeDirection(direction);
    bool correctEntry = (p.getDirection() == neededDir);

    if (!correctEntry && !p.isFlying()) {
        // Player stepped onto any part of the spring from a wrong direction -> block
        return false; // Player::move יחזיר את הצעד אחורה
    }

    // 3) From here: entry is from correct direction → we can animate the compression.

    if (onStart)
    {
        // 0) During flying / loaded – spring does nothing special
        if (p.isLoaded() || p.isFlying()) {
            start.setChar('w');
            return true;
        }
        start.setChar('_');   // compress first part
        return true;
    }

    if (onMiddle)
    {
        if (p.isLoaded() || p.isFlying()) {
            middle.setChar('w');
            return true;
        }
        middle.setChar('_');  // compress middle
        return true;
    }

    if (onEnd)
    {
        // compress entire spring
        start.setChar('_');
        middle.setChar('_');

        // load the spring → player will start flying in 'direction'
        loadSpring(p);
        return true;
    }

    return true;
}


void Spring::loadSpring(Player& p)
{
    p.stopMovement();
    p.setLoaded(true);
    p.setLaunchDirection(direction);
	p.setLoadedSpringLen(getLength());
}





