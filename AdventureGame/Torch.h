#pragma once
#include "GameObject.h"
#include "Player.h"

class Torch : public GameObject
{
public:
	Torch(int x, int y, char c, ScreenId screen)
			: GameObject(x, y, c, screen){}
};

