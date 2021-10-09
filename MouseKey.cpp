#include <stdio.h>
#include <windows.h>
#include <time.h>
#define screen_x 80
#define screen_y 25
HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD characterPos = { 2,0 };
COORD bufferSize = { screen_x,screen_y };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
int posx,posy,colisrec=0,color = 15;
COORD star = {0 + rand() % screen_x,0 + rand() % screen_y };
int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}
void ship_buffer(int x,int y,int z)
{
	consoleBuffer[(x + screen_x * y) - 2].Char.AsciiChar = '<';
	consoleBuffer[(x + screen_x * y) - 2].Attributes = z;
	consoleBuffer[(x + screen_x * y) - 1].Char.AsciiChar = '-';
	consoleBuffer[(x + screen_x * y) - 1].Attributes = z;
	consoleBuffer[x + screen_x * y].Char.AsciiChar = 'O';
	consoleBuffer[x + screen_x * y].Attributes = z;
	consoleBuffer[(x + screen_x * y) + 1].Char.AsciiChar = '-';
	consoleBuffer[(x + screen_x * y) + 1].Attributes = z;
	consoleBuffer[(x + screen_x * y) + 2].Char.AsciiChar = '>';
	consoleBuffer[(x + screen_x * y) + 2].Attributes = z;
}
void star_buffer(int x,int y)
{
	consoleBuffer[(x + screen_x * y)].Char.AsciiChar = '*';
	consoleBuffer[(x + screen_x * y)].Attributes = 5;
}
void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}
void clear_star(int x,int y) {
	consoleBuffer[(x + screen_x * y)].Char.AsciiChar = ' ';
	consoleBuffer[(x + screen_x * y)].Attributes = 15;
}
int main()
{
	srand(time(NULL));
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	while (play)
	{
		star_buffer(star.X, star.Y);
		for (int i=0 ; i < 100 ;i++)
		{
			fill_buffer_to_console();
		}
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					else if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						color = 0 + rand() % 255;
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						ship_buffer(posx, posy,color);
						fill_buffer_to_console();
						clear_buffer();
						fill_buffer_to_console();
						ship_buffer(posx, posy,color);
						fill_buffer_to_console();
						if ((star.X==posx && star.Y==posy) || (star.X == posx+2 && star.Y == posy) 
							|| (star.X == posx - 2 && star.Y == posy) || (star.X == posx - 1 && star.Y == posy)
							||(star.X == posx + 1 && star.Y == posy)) {
							clear_star(star.X, star.Y);
							star.X = 0 + rand() % screen_x;
							star.Y = 0 + rand() % screen_y;
							colisrec += 1;
							if (colisrec == 10)play = false;
						}
					}
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						color = 0 + rand() % 255;
					}
				}
			}
			delete[] eventBuffer;
		}
	}
	return 0;
}