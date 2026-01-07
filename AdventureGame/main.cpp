#include "Game.h"
#include <iostream>

int main(int argc, char* argv[])
{
    Game* game = nullptr;
    bool isSave = false;
    bool isLoad = false;
    bool isSilent = false;

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-save") isSave = true;
        else if (arg == "-load") {
            isLoad = true;
            if (argc > 2 && std::string(argv[2]) == "-silent") isSilent = true;
        }
    }

    if (isLoad) {
        game = new FileGame(isSilent);
    }
    else {
        game = new KeyBoardGame(isSave);
    }

    game->start();

    delete game;
    return 0;
}