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

/* ----------------------------------------------------
                SPRING COLLISION LOGIC
   ---------------------------------------------------- */

bool Spring::handleCollision(Player& p, const Screen& screen)
{
    int px = p.getX();
    int py = p.getY();

    bool onStart = (px == start.getX() && py == start.getY());
    bool onMiddle = (px == middle.getX() && py == middle.getY());
    bool onEnd = (px == end.getX() && py == end.getY());

    if (!onStart && !onMiddle && !onEnd)
        return true; // Should normally never happen if isAtPosition was checked

    // Must approach the spring from the opposite direction
    Direction neededDir = Spring::oppositeDirection(direction);
    bool correctEntry = (p.getDirection() == neededDir);

    // If entering from wrong direction (and not already flying) → block
    if (!correctEntry && !p.isFlying())
        return false;

    // Compression behavior
    if (onStart)
    {
        if (p.isLoaded() || p.isFlying())
        {
            start.setChar('w');   // light compression animation
            return true;
        }
        start.setChar('_');       // full compression
        return true;
    }

    if (onMiddle)
    {
        if (p.isLoaded() || p.isFlying())
        {
            middle.setChar('w');
            return true;
        }
        middle.setChar('_');
        return true;
    }

    // Reached final segment → fully compressed spring
    if (onEnd)
    {
        start.setChar('_');
        middle.setChar('_');

        // Load player for launching
        loadSpring(p);
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
