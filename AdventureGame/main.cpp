#include "Game.h"
#include <iostream>
#include <exception>
#include <conio.h> // For _getch()
#include <ctime>   // For time()

int main()
{
    // אתחול המספרים הרנדומליים פעם אחת בתחילת התוכנית
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
        catch (const std::exception& e)
        {
            system("cls");
            std::cerr << "\n\n   !!! CRITICAL ERROR !!!\n\n";
            std::cerr << "   Error Details: " << e.what() << "\n\n";
            std::cerr << "   Press any key to return to Main Menu and try again...";
            _getch();
        }
        catch (...)
        {
           
            system("cls");
            std::cerr << "\n\n   !!! UNKNOWN ERROR !!!\n\n";
            std::cerr << "   The game crashed due to an unexpected error.\n";
            std::cerr << "   Press any key to restart...";
            _getch();
        }
    }

    return 0;
}