#pragma once
#include "GameObject.h"

class Key : public GameObject
{
private:
	int keyID;
public:
	Key(int x, int y, char c, ScreenId screen,int id)
		: GameObject(x, y,'K', screen), keyID(id) {
	}

	int getKeyID() const {
		return keyID;
	}
};

