#pragma once

#include "Point.h"
#include "ScreenID.h"

// Forward declarations (avoid unnecessary includes)
class Player;
class Screen;

/*
    GameObject:
    Base class for all interactive objects in the game world.
    Handles position, screen ownership, collision rules,
    drawing, and inventory behavior.
*/
class GameObject
{
public:
    // --- Constructor & Destructor ---
    GameObject(int x, int y, char symbol,
        ScreenId screen,
        bool solid = false,
        bool collected = false)
        : point(x, y, symbol),
        screenID(screen),
        solid(solid),
        collected(collected)
    {
    }

    virtual ~GameObject() {}

    // ------------------------------------------------------------
    //                   COLLISION & LOGIC
    // ------------------------------------------------------------

    // Called when the player steps onto this object's location.
    // Return true to allow walking onto the object, false to block.
    virtual bool handleCollision(Player& p, const Screen& screen);

    // Optional action trigger (for objects with an active ability)
    virtual void use() {}

    // ------------------------------------------------------------
    //              INVENTORY & GAME STATE MANAGEMENT
    // ------------------------------------------------------------

    // Mark as collected by a player
    virtual void Collected() { collected = true; }

    // Mark as dropped into the world
    virtual void drop() { collected = false; }

    // Move object to a coordinate in the current screen
    virtual void dropToScreen(int x, int y);

    // Soft-remove from world (move off-screen)
    virtual void removeFromGame();

    // ------------------------------------------------------------
    //                       ACCESSORS
    // ------------------------------------------------------------

    virtual bool isSolid() const { return solid; }
    virtual bool isCollected() const { return collected; }

    virtual ScreenId getScreenId() const { return screenID; }
    void setScreenId(ScreenId newID) { screenID = newID; }

    int getX()  const { return point.getX(); }
    int getY()  const { return point.getY(); }
    char getChar() const { return point.getChar(); }

    Point getPoint() const { return point; }

    // Update position on the map
    void setPosition(int x, int y) { point.setPos(x, y); }

    // Check if object occupies (x,y)
    virtual bool isAtPosition(int x, int y) const
    {
        return (x == point.getX() && y == point.getY());
    }

    // Draw object directly (used in non-buffer mode)
    virtual void draw() const { point.draw(); }

protected:
    Point point;           // Location and symbol
    ScreenId screenID;     // Which room this object belongs to
    bool solid;            // Whether player can walk through
    bool collected;        // Whether inside player's inventory
};
