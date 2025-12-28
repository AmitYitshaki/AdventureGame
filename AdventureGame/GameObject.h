#pragma once
#include "Point.h"
#include "ScreenID.h"
#include <vector> 
#include <string>

class Player;
class Screen;

/*
 * ===================================================================================
 * Class: GameObject
 * -----------------------------------------------------------------------------------
 * Purpose:
 * The abstract base class for all interactive entities in the game (Keys, Doors,
 * Enemies, Bombs, etc.).
 *
 * Behavior:
 * - Defines the interface for collision handling (`handleCollision`).
 * - Manages basic properties: Position, Symbol, Visibility, and "Solid" state.
 * - Supports inventory logic (Collected/Dropped).
 *
 * Implementation:
 * - Uses virtual functions to allow polymorphism (derived classes override logic).
 * - Stores a `Point` object for location and a `ScreenId` for room association.
 * ===================================================================================
 */

class GameObject
{
public:
    GameObject(int x, int y, char symbol, ScreenId screen, bool solid = false, bool collected = false)
        : point(x, y, symbol), screenID(screen), solid(solid), collected(collected) {
    }

    virtual ~GameObject() {}

    // --- Logic ---
    virtual bool handleCollision(Player& p, const Screen& screen);
    virtual void use() {}
    virtual bool handleExplosionAt(int x, int y);
    virtual void receiveSignal(bool state) {}

    // --- Inventory & State ---
    virtual void Collected() { collected = true; }
    virtual void drop() { collected = false; }
    virtual void dropToScreen(int x, int y);
    virtual void removeFromGame();

    // --- Drawing ---
    virtual void drawToBuffer(std::vector<std::string>& buffer) const;
    virtual void draw() const { point.draw(); }

    // --- Accessors ---
    virtual bool isSolid() const { return solid; }
    virtual bool isCollected() const { return collected; }
    virtual ScreenId getScreenId() const { return screenID; }
    void setScreenId(ScreenId newID) { screenID = newID; }

    int getX() const { return point.getX(); }
    int getY() const { return point.getY(); }
    char getChar() const { return point.getChar(); }
    Point getPoint() const { return point; }

    void setPosition(int x, int y) { point.setPos(x, y); }
    void setChar(char newC) { point.setChar(newC); }

    virtual bool isAtPosition(int x, int y) const { return (x == point.getX() && y == point.getY()); }

protected:
    Point point;
    ScreenId screenID;
    bool solid;
    bool collected;
};