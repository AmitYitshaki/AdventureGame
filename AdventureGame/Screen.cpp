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
	"#                                                                              #",// 3
	"#                             ADVENTURE GAME                                   #",// 4
	"#                                                                              #",// 5
	"#                                                                              #",// 6
	"#                                                                              #",// 7
	"#                           1 - Start New Game                                 #",// 8
	"#                           2 - Instructions                                   #",// 9
	"#                           3 - Quit                                           #",// 10
	"#                                                                              #",// 11
	"#                                                                              #",// 12
	"#                                                                              #",// 13
	"#                                                                              #",// 14
	"#                                                                              #",// 15
	"#                                                                              #",// 16
	"#                                                                              #",// 17
	"#                                                                              #",// 18
	"#                                                                              #",// 19
	"#                                                                              #",// 20
	"#                                                                              #",// 21
	"#                                                                              #",// 22
	"#                                                                              #",// 23
	"################################################################################"// 24
};

const char* Screen::INSTRUCTIONS_LAYOUT[HEIGHT] = {
   //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	"###############################################################################",
	"#                                                                             #",
	"#                           ADVENTURE GAME - INSTRUCTIONS                     #",
	"#                                                                             #",
	"#  Goal:                                                                      #",
	"#    Be the first player to reach the target or survive longer than your      #",
	"#    opponent while avoiding walls and staying inside the playground.         #",
	"#                                                                             #",
	"#  Controls:                                                                  #",
	"#    Player 1  ($):  W = up,   X = down,  A = left,  D = right,  S = stay     #",
	"#    Player 2  (&):  I = up,   M = down,  J = left,  L = right,  K = stay     #",
	"#                                                                             #",
	"#  Keys during game:                                                          #",
	"#    ESC  - pause game                                                        #",
	"#    H    - open Home screen(when available)                                  #",
	"#                                                                             #",
	"#  Rules:                                                                     #",
	"#    - You cannot move through walls (#).                                     #",
	"#    - Moving outside the frame is not allowed.                               #",
	"#                                                                             #",
	"#  Tips:                                                                      #",
	"#    - Think a few moves ahead and watch your opponent.                       #",
	"#                                                                             #",
	"#  Press H to return to the Home Screen.                                      #",
	"###############################################################################",
};

const char* Screen::ROOM1_LAYOUT[HEIGHT] = {
   //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	"p1 Lives: Item:   p2 Lives: Item:  room:1                                       ",
	"################################################################################",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                               ",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"#                                                                              #",
	"################################################################################"
};

const char* Screen::ROOM2_LAYOUT[HEIGHT] = {
   //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	"p1 Lives: Item:   p2 Lives: Item:  room:2                                       ",
	"################################################################################",
	"#                                               #                              #",
	"#                   ###########       ###########                              #",
	"#                   #         #       #         #                              #",
	"#                   #         #       #         #                              #",
	"#                   #         #########         #                              #",
	"#                   #                           #                              #",
	"#                   #                           #                              #",
	"#                   #########   #   #########   #                              #",
	"#                               #                                              #",
	"#                             # # #                                            #",
	"#                   ######### # # # #########   #                               ",
	"#                   #         # # #         #   #                              #",
	"#                   #         # # #         #   #                              #",
	"#                   #         # # #         #   #                              #",
	"#                   ########### # ###########   #                              #",
	"#                               #               #                              #",
	"#                               #               #                              #",
	"#                               #               #                              #",
	"#                               ############### #                              #",
	"#                                             # #                              #",
	"#                                             # #                              #",
	"#                                             # #                              #",
	"################################################################################"
};

const char* Screen::ROOM3_LAYOUT[HEIGHT] = {
	"p1 Lives: Item:   p2 Lives: Item:  room:3                                      ",
	"###############################################################################",
	"#                                                                             #",
	"#        #########                                                            #",
	"#        #       #                                                            #",
	"#        #       #                                                            #",
	"#        #       #########                                                    #",
	"#                        #                                                    #",
	"#                        #                                                    #",
	"#        #########       #        #########                                   #",
	"#        #       #       #        #       #                                   #",
	"#        #       #       #        #       #                                   #",
	"#        #       #########        #########                                    ",
	"#        #                                                                    #",
	"#        #########                                                            #",
	"#                                                                             #",
	"#                                                                             #",
	"#                                 #########                                   #",
	"#                                 #       #                                   #",
	"#                                 #       #                                   #",
	"#                                 #########                                   #",
	"#                                                                             #",
	"#                                                                             #",
	"#                                                                             #",
	"###############################################################################"
};


