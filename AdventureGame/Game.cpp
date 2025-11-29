#include "Game.h"
#include <conio.h>
using namespace std;

void Game::draw()
{
    currentScreen->draw();
    // Fix: Use screens array and correct syntax for comparison
    if (currentScreen == &screens[(int)ScreenId::HOME] || currentScreen == &screens[(int)ScreenId::INSTRUCTIONS])
    {
        return;
    }
    player1.draw();
    player2.draw();

	cout.flush();
}

void Game::handleInput()
{
	if (_kbhit())
	{
		char key = _getch();
		if (currentScreen == &screens[(int)ScreenId::HOME] && key == '1') //start new game
        {
			player1.updatepoint(3, 3); //initialize players positions
			player2.updatepoint(3, 4);
			player1.setDirection(Direction::STAY); //initialize players directions
			player2.setDirection(Direction::STAY);
            goToScreen(ScreenId::ROOM1); 
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::HOME] && key == '2')
        {
            goToScreen(ScreenId::INSTRUCTIONS);
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::HOME] && key == '3')
        {
            end();
            return;
        }
        else if (currentScreen == &screens[(int)ScreenId::INSTRUCTIONS] && (key == 'H' || key == 'h'))
        {
            goToScreen(ScreenId::HOME);
            return;
		}
        else if (key == 27) // ESC key
		{
			pauseScreen(); 
            return;
		}
		else
		{
			ChangeDirection(key);  
            return;
		}
	}
}

void Game::ChangeDirection(char c)
{
    // ----- Player 1 -----
    if (c == 'w' || c == 'W')
    {player1.setDirection(Direction::UP);}
    else if (c == 'x' || c == 'X')
    {player1.setDirection(Direction::DOWN);}
    else if (c == 'a' || c == 'A')
    {player1.setDirection(Direction::LEFT);}
    else if (c == 'd' || c == 'D')
    {player1.setDirection(Direction::RIGHT);}
    else if (c == 's' || c == 'S')
    {player1.setDirection(Direction::STAY);}

    // ----- Player 2 -----
    else if (c == 'i' || c == 'I')
    {player2.setDirection(Direction::UP);}
    else if (c == 'm' || c == 'M')
    {player2.setDirection(Direction::DOWN);}
    else if (c == 'j' || c == 'J')
    {player2.setDirection(Direction::LEFT);}
    else if (c == 'l' || c == 'L')
    {player2.setDirection(Direction::RIGHT);}
    else if (c == 'k' || c == 'K')
    {player2.setDirection(Direction::STAY);}
}


void Game::start() {
	hideCursor();
	while (isRunning) {
		handleInput();
        update();
		draw();
		Sleep(250);
	}
	cls();
}

void Game::pauseScreen()
{
    int x = (Screen::WIDTH - 37) / 2;
    int y = (Screen::HEIGHT - 5) / 2;

    gotoxy(x, y);     cout << "*************************************";
    gotoxy(x, y + 1); cout << "* Game Paused                       *";
    gotoxy(x, y + 2); cout << "* Press ESC to continue             *";
    gotoxy(x, y + 3); cout << "* Press H to open Home Screen       *";
    gotoxy(x, y + 4); cout << "*************************************";

    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();
            if (key == 27) // ESC key
            {
                return;
            }
            else if (key == 'H' || key == 'h') // H or h key
            {
                goToScreen(ScreenId::HOME);
				return;
            }
        }
    }
}

void Game::stopMovement()
{
	player1.setDirection(Direction::STAY);
	player2.setDirection(Direction::STAY);
}


