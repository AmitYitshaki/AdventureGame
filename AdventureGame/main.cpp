#include "Game.h"
#include "GameException.h" // <--- הוספה
#include <iostream>
#include <conio.h>
#include <ctime>

int main()
{
    srand((unsigned int)time(NULL));
    bool keepRunning = true;

    while (keepRunning)
    {
        try
        {
            Game game;
            game.start();
            keepRunning = false; 
        }
        catch (const GameException& e)
        {
            system("cls");
            std::cerr << "\n======================================\n";
            std::cerr << "         CRITICAL GAME ERROR          \n";
            std::cerr << "======================================\n";
            std::cerr << e.what() << "\n";
            std::cerr << "======================================\n";
            std::cerr << "\nPress any key to restart safely...";
            _getch();
        }
        catch (const std::exception& e)
        {
            system("cls");
            std::cerr << "SYSTEM ERROR: " << e.what() << "\n";
            std::cerr << "Press any key to exit...";
            _getch();
            return 1;
        }
    }
    return 0;
}