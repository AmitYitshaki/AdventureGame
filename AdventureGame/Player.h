#pragma once
#include "Point.h"
#include "Direction.h"
#include "ScreenID.h"
#include "Torch.h"
#include <vector>

class Screen;
class GameObject;
class Torch;

class Riddle; // Forward declaration

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
	int springTicksLeft = 0;
	int lastLoadedSpringLength = 0;
	Direction launchDirection = Direction::STAY;
	GameObject* heldItem = nullptr;
	ScreenId currentLevel = ScreenId::ROOM1;

	

public:
	Player(int x, int y, char c, int HudPos) //member initializer list
		: point(x, y, c), hudX(HudPos), flying(false) {
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
	void launch(int springLen);
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
	void decreaseLife(); 
	int getHudX() const {
		return hudX;
	}
	int getHudY() const {
		return hudY;
	}
	void startSpringEffect(int power);   // power = N (אורך הקפיץ)
	void updateSpringEffect();           // יוריד טיק אחד בכל update של המשחק
	void stopSpringEffect();             // מאפס מצב קפיץ (כשהטיימר נגמר או יש התנגשות)

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

	bool hasTorch() const;

	bool hasRiddle() const;
	Riddle* getHeldRiddle() const;
	int getLive() const {
		return live;
	}
	void launch(Direction d);
	int getLoadedSpringLen() const { return lastLoadedSpringLength; }
	void setLoadedSpringLen(int len) { lastLoadedSpringLength = len; }

	
};



