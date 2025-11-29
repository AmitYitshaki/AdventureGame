#pragma once
#include "Player.h"
#include "Screen.h"
#include "ScreenID.h"

class Game
{
	private:
		Player player1;
		Player player2; 
		Screen screens[(int)ScreenId::NumOfScreens];   // array of screens
		Screen* currentScreen; // pointer to the current screen
	
		bool isRunning;
public:
	Game() //constructor 
		: player1(3, 3, '$'), player2(3, 4, '&'), isRunning(true) //initialize players and isRunning
	{ //creating screens and screens array and setting the current screen to HOME
			screens[(int)ScreenId::HOME] = Screen (ScreenId::HOME);
			screens[(int)ScreenId::INSTRUCTIONS] = Screen(ScreenId::INSTRUCTIONS);
			screens[(int)ScreenId::ROOM1] = Screen(ScreenId::ROOM1);
			screens[(int)ScreenId::ROOM2] = Screen(ScreenId::ROOM2);
			screens[(int)ScreenId::ROOM3] = Screen(ScreenId::ROOM3);

			currentScreen = &screens[(int)ScreenId::HOME];
	}
	void draw();

	void update() {
		player1.move(*currentScreen);
		player2.move(*currentScreen);
	}
	bool running() const {
		return isRunning;
	}
	void end() {
		isRunning = false;
	}

	void handleInput();
	void ChangeDirection(char c);
	void start();
	void pauseScreen();
	void stopMovement();
	void goToScreen(ScreenId screenID) {
		currentScreen = &screens[(int)screenID];
	}

};

