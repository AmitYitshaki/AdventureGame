#include "Game.h"
#include "GameException.h" 
#include <iostream>
#include <conio.h>
#include <ctime>

int main(int argc,char* argv[])
{
    RunMode mode = RunMode::Normal;
    bool isSilent = false;

    if (argc > 1)
    {
        std::string modeArg = argv[1];
        if (modeArg == "-save") {
            mode = RunMode::Save;
        }
        else if (modeArg == "-load") {
            mode = RunMode::Load;
        }
        if (argc > 2 && mode == RunMode::Load) {
            std::string silentArg = argv[2];
            if (silentArg == "-silent") {
                isSilent = true;
                mode = RunMode::Silent;
            }
        }
    }

    srand((unsigned int)time(NULL));
    bool keepRunning = true;

    while (keepRunning)
    {
        try
        {
            Game game(mode);
            game.start();
            keepRunning = false;
        }
        catch (const GameException& e)
        {
            if (mode == RunMode::Silent || mode == RunMode::Load) {
                std::cerr << "Test Failed: " << e.what() << "\n";
                return 1;
			}
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