#include "GameObject.h"
#include "Player.h" // Required to use Player methods (collectItem)

void GameObject::dropToScreen(int x, int y)
{
	// Update position to the player's current location
	point.setpoint(x, y);
	// Mark as available for pickup
	collected = false;
}

void GameObject::removeFromGame()
{
	// "Soft Delete": Move object off-screen to effectively remove it
	setPosition(-1, -1);
}

bool GameObject::handleCollision(Player& p, const Screen& screen)
{
	// Case 1: Non-solid object (e.g., Key, Bomb) -> Walk through & Collect
	if (!isSolid())
	{
		// Only collect if it's not already in inventory
		if (!getIsCollected()) {
			p.collectItem(this);
		}
		return true; // Allow movement
	}

	// Case 2: Solid object (e.g., Wall, Rock) -> Block movement
	// Derived classes (like Door) override this to add conditional entry logic.
	return false;
}