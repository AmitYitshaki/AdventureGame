#pragma once
#include "Player.h"
#include "Screen.h"
#include "ScreenID.h"
#include "GameObject.h"
#include "Key.h"
#include "Door.h"
#include "spring.h"
#include "Torch.h"
#include <vector>
#include <string>
#include "Riddle.h"

class Game
{
private:
	Player player1;
	Player player2;
	Screen screens[(int)ScreenId::NumOfScreens];
	Screen* currentScreen;


	// Vector to hold all game objects
	std::vector<GameObject*> gameObjects;

	
	bool isRunning;
	bool RiddleMode = false;
	Riddle* currentRiddle = nullptr;
	Player* currentRiddlePlayer = nullptr;
	static std::string statusMessage;

	// --- Private Helper Functions (Logic) ---
	void resetPlayersForNewLevel();
	void checkLevelTransition();

	// --- Private Helper Functions (Double Buffering / Draw) ---
	std::vector<std::string> initBuffer();
	void writeToBuffer(std::vector<std::string>& buffer, int x, int y, char c);
	void writeTextToBuffer(std::vector<std::string>& buffer, int x, int y, const std::string& text);

	void drawObjectsToBuffer(std::vector<std::string>& buffer);
	void drawPlayersToBuffer(std::vector<std::string>& buffer);
	void drawStatusToBuffer(std::vector<std::string>& buffer);
	void drawRiddle(std::vector<std::string>& buffer);
	void renderBuffer(const std::vector<std::string>& buffer);



public:
	// Constructor
	Game()
		: player1(2, 12, '$', 16), player2(3,12, '&', 33), isRunning(true)
	{
		// Initialize screens
		screens[(int)ScreenId::HOME] = Screen(ScreenId::HOME);
		screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
		screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
		screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
		screens[(int)ScreenId::ROOM2].setDark(true); // Make ROOM2 dark initially
		screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);

		currentScreen = &screens[(int)ScreenId::HOME];

		// Initialize objects (Keys and Doors)
		// Note: Door location set to 78 to fit inside 0-79 bounds
		gameObjects = {
			new Key(15, 20, 'K', ScreenId::ROOM1, 1),
			new Riddle(30, 5, ScreenId::ROOM1, RiddleId::RIDDLE1),
			new Spring(Point(4,15, 'w'),Point(3,15, 'w'),Point(2,15, 'W'),Direction::RIGHT,ScreenId::ROOM1),
			new Spring(Point(76,17, 'w'),Point(77,17,'w'),Point(78,17,'W'),Direction::LEFT,ScreenId::ROOM1),
			new Door(79, 12, '1', ScreenId::ROOM1, 1, ScreenId::ROOM2, true),
			new Key(20, 15, 'K', ScreenId::ROOM2, 2),
			new Torch(15, 10, '!', ScreenId::ROOM2),
			new Torch(14, 10, '!', ScreenId::ROOM2),
			new Riddle(77, 12, ScreenId::ROOM2, RiddleId::RIDDLE2),
			new Door(79, 12, '2', ScreenId::ROOM2, 2, ScreenId::ROOM3, true)
			

		};
	}

	// Destructor (Implementation should be in .cpp to delete objects)
	~Game();

	void draw();
	void update();

	bool running() const { return isRunning; }
	void end() { isRunning = false; }

	void handleInput();
	void ChangeDirection(char c);
	void start();
	void pauseScreen();
	void stopMovement();
	void checkIsPlayerLoaded();
	void handleRiddleInput(char key);

	void goToScreen(ScreenId screenID) {
		currentScreen = &screens[(int)screenID];
	}
	void applyLighting(std::vector<std::string>& buffer);
	bool isRiddleMode() const {return RiddleMode;}
	void setRiddleMode(bool mode) { RiddleMode = mode;}
	void startRiddle(Riddle* riddle , Player& p);

	bool checkPlayerHasRiddle(); 
	static void setStatusMessage(const std::string& msg);
};