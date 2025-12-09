#include "Screen.h"
#include <iostream>

/*
    Screen.cpp
    Holds the ASCII layouts of all screens and implements
    basic drawing and initialization logic.
*/

Screen::Screen() // Default constructor → HOME screen
{
    screenID = ScreenId::HOME;
    layout = HOME_LAYOUT;
}

Screen::Screen(ScreenId id)
    : screenID(id)
{
    switch (id)
    {
    case ScreenId::HOME:
        layout = HOME_LAYOUT;
        break;
    case ScreenId::INSTRUCTIONS:
        layout = INSTRUCTIONS_LAYOUT;
        break;
    case ScreenId::ROOM1:
        layout = ROOM1_LAYOUT;
        break;
    case ScreenId::ROOM2:
        layout = ROOM2_LAYOUT;
        break;
    case ScreenId::ROOM3:
        layout = ROOM3_LAYOUT;
        break;
    default:
        // Fallback to HOME for unexpected values
        layout = HOME_LAYOUT;
        break;
    }
}

void Screen::draw() const
{
    gotoxy(0, 0);
    for (int i = 0; i < HEIGHT; ++i)
    {
        std::cout << layout[i];
        if (i < HEIGHT - 1)
            std::cout << '\n';
    }
}

/* ============================================================
                        STATIC LAYOUTS
   ============================================================ */

const char* Screen::HOME_LAYOUT[HEIGHT] = {
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    "################################################################################",
    "#                                                                              #",
    "#                                                                              #",
    "#             _____________________________________________________            #",
    "#            / \\                                                    \\          #",
    "#           |   |                                                   |          #",
    "#            \\__|                                                   |          #",
    "#               |         ~ A D V E N T U R E   G A M E ~           |          #",
    "#               |                                                   |          #",
    "#               |      \"Greatness awaits those who dare...\"         |          #",
    "#               |                                                   |          #",
    "#               |            1 - Start New Game                     |          #",
    "#               |            2 - Instructions                       |          #",
    "#               |            3 - Quit                               |          #",
    "#               |                                                   |          #",
    "#               |    ____________________________________________________      #",
    "#               |  /                                                   /      #",
    "#               \\_/___________________________________________________/       #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "#                                                                              #",
    "################################################################################"
};

const char* Screen::INSTRUCTIONS_LAYOUT[HEIGHT] = {
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    "################################################################################",
    "#                                                                              #",
    "#                         ADVENTURE GAME - INSTRUCTIONS                        #",
    "#                                                                              #",
    "#  Goal:                                                                       #",
    "#    Both players ($ and &) must cooperate, move through rooms and use items   #",
    "#    to reach the final room.                                                  #",
    "#                                                                              #",
    "#  Controls:                                                                   #",
    "#    Player 1 ($): W=up  X=down  A=left  D=right  S=stay  E=drop item          #",
    "#    Player 2 (&): I=up  M=down  J=left  L=right  K=stay  O=drop item          #",
    "#                                                                              #",
    "#  Movement & Walls:                                                           #",
    "#    Direction continues until wall or STAY. No moving through walls.          #",
    "#                                                                              #",
    "#  Items:                                                                      #",
    "#    spring - launches the player ahead.                                       #",
    "#    Keys - open matching doors ; torch - expands light in dark rooms.         #",
    "#    riddle - must to be PICKED UP by the players, wrong answer loses 1 life.  #",
    "#                                                                              #",
    "#  Pause/Home:                                                                 #",
    "#    ESC pauses; another ESC=continue, H= return to Home screen.               #",
    "#                                GOOD LUCK!                                    #",
    "#                                                                              #",
    "################################################################################"
};

const char* Screen::ROOM1_LAYOUT[HEIGHT] = {
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    "p1 Lives:  Item:   p2 Lives:  Item:  room:1                                     ",
    "################################################################################",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                       #                             #",
    "#                        #                                                     #",
    "#                        #                                                     #",
    "#                        #               #                                      ",
    "#                                        #                                     #",
    "#                                        #                             #########",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "#                                        #                                     #",
    "################################################################################"
};

const char* Screen::ROOM2_LAYOUT[HEIGHT] = {
    //01234567890123456789012345678901234567890123456789012345678901234567890123456789
    "p1 Lives:  Item:   p2 Lives:  Item:  room:2                                     ",
    "################################################################################",
    "#																#               #",
    "#   ###########             ###########                        #               #",
    "#   #         #             #         #                        #               #",
    "#   #         #             #         #		 #		        #               #",
    "#   # `	   #######       #         #######   ######    ######               #",
    "#   #                       #                   #              #               #",
    "#   #                                                          #               #",
    "#   #   #########       #####   #########    #                 #               #",
    "#   #           #                            #         #       #               #",
    "#               #   ###########   #####      ###########       #     ###########",
    "#                                                      #                        ",
    "#   #######          #######                           #######       ###########",
    "#   #     #          #                                 #     #                 #",
    "#   #     ######     #     #####           #####       #     ######            #",
    "#   #          #           #   #                                  #            #",
    "#   ########   ########    #   #####   #####   ########           #            #",
    "#                          #                                                   #",
    "#           #######        #########   ##############          ####            #",
    "#                 #                                               #      #     #",
    "#				   #   #############################################      #     #",
    "#   #             #                                                      #     #",
    "#   #             #                                                      #     #",
    "################################################################################",
};

const char* Screen::ROOM3_LAYOUT[HEIGHT] = {
    "p1 Lives:  Item:   p2 Lives:  Item:  room:3                                     ",
    "################################################################################",
    "#                                                                              #",
    "#                                                                              #",
    "#                            * CONGRATULATIONS! *                              #",
    "#                                                                              #",
    "#                                ___________                                   #",
    "#                               '._==_==_=_.'                                  #",
    "#                               .-\\:      /-.                                  #",
    "#                              | (|:.     |) |                                 #",
    "#                               '-|:.     |-'                                  #",
    "#                                 \\::.    /                                    #",
    "#                                  '::. .'                                     #",
    "#                                    ) (                                       #",
    "#                                  _.' '._                                     #",
    "#                                 `-------`                                    #",
    "#                                                                              #",
    "#                      YOU HAVE CONQUERED THE ADVENTURE!                       #",
    "#                                                                              #",
    "#                                                                              #",
    "#                        1 - Play Again (New Game)                             #",
    "#                        2 - Return to Main Menu                               #",
    "#                        3 - Quit Game                                         #",
    "#                                                                              #",
    "################################################################################"
};
