#pragma once
#include "GameObject.h"

class Player;

class Door : public GameObject
{
private:
	int DoorID; // The ID of the key that unlocks this door
	bool locked; // Indicates whether the door is locked
	ScreenId leadsTo; // The screen this door leads to
public:
	Door(int x, int y, char c, ScreenId screen, int id, ScreenId leadsToScreen, bool isLocked);

	bool isLocked() const {
		return locked;
	}
	ScreenId getLeadsTo() const {
		return leadsTo;
	}
	bool unlock(Player& player); // Attempt to unlock the door with the player's key
	bool handleCollision(Player& p, const Screen& screen);
};

