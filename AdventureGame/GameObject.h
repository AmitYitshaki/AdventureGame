#pragma once
#include "Point.h"
#include "ScreenID.h"
#include <vector>

// Forward declarations to avoid circular include dependency
class Player;
class Screen;

class GameObject
{
protected:
	Point point;
	ScreenId screenID;
	bool solid;     // If true -> blocks player movement (like a wall/door)
	bool collected; // If true -> currently in player's inventory

public:
	// Constructor: Initializes position, screen, and properties
	GameObject(int x, int y, char c, ScreenId screen, bool solid = false, bool collected = false)
		: point(x, y, c), screenID(screen), solid(solid), collected(collected) {
	}

	// Virtual Destructor: Essential for proper cleanup of derived classes
	virtual ~GameObject() {}

	// --- Virtual Logic Functions ---

	// Action performed when using the item (e.g., Bomb exploding)
	virtual void use() {}

	// Logic for collision with player. 
	// Returns true if player can move into this spot, false if blocked.
	virtual bool handleCollision(Player& p, const Screen& screen);

	// --- Inventory Management Functions ---

	// Updates state when collected by player
	virtual void Collected() {
		collected = true;
	}

	// Updates state when dropped by player
	virtual void drop() {
		collected = false;
	}

	// Logic to place the object back on the map at specific coordinates
	virtual void dropToScreen(int x, int y);

	// Logic to logically remove object from game (e.g. move to -1, -1)
	virtual void removeFromGame();

	// --- Getters & Setters ---

	virtual bool isSolid() const {
		return solid;
	}

	// Getter for collected state
	virtual bool isCollected() const {
		return collected;
	}
	// Redundant getter (kept for compatibility with existing code)
	virtual bool getIsCollected() const {
		return collected;
	}

	virtual void draw() const
	{
		point.draw();
	}

	virtual ScreenId getScreenId() const
	{
		return screenID;
	}

	// --- Point Delegation (Accessing internal Point members) ---

	int getX() const
	{
		return point.getX();
	}

	int getY() const
	{
		return point.getY();
	}

	void setPosition(int x, int y)
	{
		point.setpoint(x, y);
	}

	// Helper to get the character representation (for Double Buffering)
	char getChar() const {
		return point.getChar();
	}

	// Helper to return the point object itself
	Point getPoint() const {
		return point;
	}

	// Crucial for moving objects between screens (e.g. dropping a key in a new room)
	void setScreenId(ScreenId newID) {
		screenID = newID;
	}
};