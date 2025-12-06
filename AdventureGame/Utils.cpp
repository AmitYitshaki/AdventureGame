#include "Utils.h"


void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	std::cout.flush();
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(hStdOut, &curInfo);
	curInfo.bVisible = FALSE; // Set to TRUE to make it visible
	SetConsoleCursorInfo(hStdOut, &curInfo);
}

void cls() {
	system("cls");
}

