#include "Screen.h"

Screen::Screen() //defult ctor
{
	layout = HOME_LAYOUT;
};

Screen::Screen(ScreenId screenID) : screenID(screenID)//screen paramter ctor
{
	switch (screenID)
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
		layout = HOME_LAYOUT; // הגנה למקרה של ערך לא צפוי
		break;
	}
}

void Screen::draw() const {
	gotoxy(0, 0);
	for (int i = 0; i < HEIGHT; ++i) {
		std::cout << layout[i];
		if (i < HEIGHT - 1)
			std::cout << '\n';
	}
}


const char* Screen::HOME_LAYOUT[HEIGHT] ={
   //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	"################################################################################",// 0
	"#                                                                              #",// 1
	"#                                                                              #",// 2
	"#             _____________________________________________________            #",// 3
	"#            / \\                                                    \\          #",// 4
	"#           |   |                                                   |          #",// 5
	"#            \\__|                                                   |          #",// 6
	"#               |         ~ A D V E N T U R E   G A M E ~           |          #",// 7
	"#               |                                                   |          #",// 8
	"#               |      \"Greatness awaits those who dare...\"         |          #",// 9
	"#               |                                                   |          #",// 10
	"#               |            1 - Start New Game                     |          #",// 11
	"#               |            2 - Instructions                       |          #",// 12
	"#               |            3 - Quit                               |          #",// 13
	"#               |                                                   |          #",// 14
	"#               |    ____________________________________________________      #",// 15
	"#               |  /                                                   /      #",// 16
	"#               \\_/___________________________________________________/       #",// 17
	"#                                                                              #",// 18
	"#                                                                              #",// 19
	"#                                                                              #",// 20
	"#                                                                              #",// 21
	"#                                                                              #",// 22
	"#                                                                              #",// 23
	"################################################################################" // 24
};



const char* Screen::INSTRUCTIONS_LAYOUT[HEIGHT] = 
{
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
	"#    riddle - must to be picked up by the players, wrong answer loses 1 life.  #",
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
"#                               .-\\:      /-.                                 #",
"#                              | (|:.     |) |                                 #",
"#                               '-|:.     |-'                                  #",
"#                                 \\::.    /                                   #",
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



