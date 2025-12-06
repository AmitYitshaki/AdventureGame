#pragma once
#include "Point.h"
#include "Direction.h"
#include "ScreenID.h"
#include <vector>

class Screen;
class GameObject;

class Player
{
private:
	Point point;
	Direction dir = Direction::STAY;
	int hudX;
	int hudY = 0;
	int speed = 1;
	int live = 3;
	bool flying = false;
	bool loaded = false;
	Direction launchDirection = Direction::STAY;
	GameObject* heldItem = nullptr;
	ScreenId currentLevel = ScreenId::ROOM1;

	

public:
	Player(int x, int y, char c, int HudPos) //member initializer list
		: point(x, y, c), hudX(HudPos), flying(flying) {
	}

	~Player();

	void draw() const
	{
		point.draw();
	}
	void drawNoSleep() const
	{
		point.drawNoSleep();
		
	}

	//move functions:
	void setDirection(Direction direction)
	{
		dir = direction;
	}
	void stopMovement()
	{
		dir = Direction::STAY;
	}
	int getX() const
	{
		return point.getX();
	}
	int getY() const
	{
		return point.getY();
	}
	void move(Screen& screen,std::vector<GameObject*>& gameObjects);
	void moveFlying(Screen& screen, std::vector<GameObject*>& gameObjects);
	void launch();
	Direction getDirection() const
	{
		return dir;
	}
	void updatepoint(int x, int y)
	{
		point.setpoint(x, y);
	}
	char getChar() const
	{
		return point.getChar();
	}
	ScreenId getCurrentLevel() const { return currentLevel; }
	void setCurrentLevel(ScreenId id) { currentLevel = id; }
	bool hasItem() const
	{
		return heldItem != nullptr;
	}
	bool isFlying() const
	{
		return flying;
	}
	void setFlying(bool canFly) {
		flying = canFly;
	}

	bool isLoaded() const {
		return loaded;
	}
	void setLoaded(bool isLoaded) {
		loaded = isLoaded;
	}
	void setLaunchDirection(Direction d) {
		launchDirection = d;
	}

	Direction getLaunchDirection() const {
		return launchDirection;
	}

	bool collectItem(GameObject* item);

	GameObject* dropItemToScreen(ScreenId currentScreenID);
	void removeHeldItem();

	GameObject* getHeldItem()
	{
		return heldItem;
	}

	int getHeldKeyID() const;

	void launch(Direction d);

	
};



