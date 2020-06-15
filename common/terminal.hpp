#ifndef TERM_INFO_H
#define TERM_INFO_H

#ifdef _WIN32
#include <stdint.h>
#include <windows.h>

void SetTerminalCursorPosition(uint8_t x, uint8_t y) {
	_COORD coord;

	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

#endif

// TODO: add LINUX implemetation

#endif // TERM_INFO_H
