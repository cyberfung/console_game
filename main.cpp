#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <time.h>

#include <iostream>

#ifndef TARGETOSLINUX
#include <conio.h>
#include <windows.h>
#endif

using namespace std;

#ifndef TARGETOSLINUX
void cls( HANDLE hConsole );

HANDLE hStdout;
#endif

// Linux specific overrides
#ifdef TARGETOSLINUX
#define getch() getc(stdin)
#define setCursorPosition(a, b) 0
#define enterRawMode() system("stty raw")
#define enterNormalMode() system("stty cooked")
#endif

class Map {
	public:
		char **matrix;
		int w, h, money;

		Map () : w(0), h(0), money(0) {}
		Map (int width, int height) : w(width), h(height), money(0) {
			init(w, h);
		}
		~Map () {
			for (int i = 0; i < h; i++) {
				delete(matrix[i]);
			}
			delete(matrix);
		}

		void init (int width, int height);

		void draw();

	private:
		void createMatrix (int width, int height);

		void initMatrix ();

		void generateHoles ();

		void generateMoney ();



};


void Map::createMatrix (int width, int height) {
	unsigned int y;

	matrix = (char**) malloc(sizeof(*matrix) * height);

	for (y = 0; y < height; y++) {
		matrix[y] = (char*) malloc(sizeof(**matrix) * width);
	}

	w = width;
	h = height;
}

void Map::initMatrix () {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			matrix[y][x] = '.';
		}
	}
}

void Map::generateHoles () {
	int xRand, yRand, holes;

	holes = w * h / 20;

	for (int i = 0; i < holes; i++) {
		do {
			xRand = rand() % w;
			yRand = rand() % h;
		} while (matrix[yRand][xRand] == '@');

		matrix[yRand][xRand] = '@';
	}
}

void Map::generateMoney ()  {
	int xRand, yRand, money;

	money = w * h / 20;
	this->money = money;

	for (int i = 0; i < money; i++) {
		do {
			xRand = rand() % w;
			yRand = rand() % h;
		} while (matrix[yRand][xRand] == '@' || matrix[yRand][xRand] == '$');

		matrix[yRand][xRand] = '$';
	}
}

void Map::init (int width, int height) {
	createMatrix(width, height);
	initMatrix();

	generateHoles();
	generateMoney();
}

void Map::draw () {
	for (int i = 0; i < w + 2; i++) {
		printf("#");
	}
	printf("\n");

	for (int y = 0; y < h; y++) {
		printf("#");
		for (int x = 0; x < w; x++) {
			printf("%c", matrix[y][x]);
		}
		printf("#");
		printf("\n");
	}

	for (int i = 0; i < w + 2; i++) {
		printf("#");
	}
	printf("\n");
}

class Hero {
	public:
		int x, y, money;

		Hero () : x(0), y(0), money(0) {}

		void init(int w, int h);

		int move (Map &map);
};

void Hero::init(int w, int h) {
	x = rand() % w;
	y = rand() % h;
}

int Hero::move (Map &map) {
	switch (map.matrix[y][x]) {
		case '@':
			return(1);
		case '$':
			money += 1;
			map.matrix[y][x] = '*';
			map.money -= 1;
		default:
			map.matrix[y][x] = '*';
	}
	return(0);
}


void screenClear () {
#ifndef TARGETOSLINUX
	cls(hStdout);
#else
  system("clear");
#endif
}

#ifndef TARGETOSLINUX
void cls( HANDLE hConsole ) {
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.

	if( !GetConsoleScreenBufferInfo( hConsole, &csbi )) {
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.

	if( !FillConsoleOutputCharacter( hConsole,        // Handle to console screen buffer
				    (TCHAR) ' ',     // Character to write to the buffer
				    dwConSize,       // Number of cells to write
				    coordScreen,     // Coordinates of first cell
				    &cCharsWritten )) { // Receive number of characters written

		return;
	}

	// Get the current text attribute.

	if( !GetConsoleScreenBufferInfo( hConsole, &csbi )) {
		return;
	}

	// Set the buffer's attributes accordingly.

	if( !FillConsoleOutputAttribute( hConsole,         // Handle to console screen buffer
				    csbi.wAttributes, // Character attributes to use
				    dwConSize,        // Number of cells to set attribute
				    coordScreen,      // Coordinates of first cell
				    &cCharsWritten )) { // Receive number of characters written

		return;
	}

	// Put the cursor at its home coordinates.

	SetConsoleCursorPosition( hConsole, coordScreen );
}
#endif

#ifndef TARGETOSLINUX
void setCursorPosition(int x, int y) {
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}
#endif

int main (int argc, char *argv[]) {
	char move;
	char heroSymbol = '*';
	int isOver = 0;

#ifndef TARGETOSLINUX
	//winPart
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
#endif


	srand(time(NULL));

	Map map(30, 20);
	Hero hero;
	do {
		hero.init(map.w, map.h);
	} while (map.matrix[hero.y][hero.x] == '$' || map.matrix[hero.y][hero.x] == '@');
	map.matrix[hero.y][hero.x] = heroSymbol;

	screenClear();
	printf("for move: w - up, s - down, a - left, d - right and ENTER. q - quit\n");
	printf("Money = %d\n", hero.money);
	map.draw();
	fflush(stdout);

	while (1) {
		if (isOver != 1) {
			setCursorPosition(0, 4 + map.h);

      enterRawMode();
			move = getch();
      enterNormalMode();

			if (move == 'a') {
				if (hero.x > 0) {
					setCursorPosition(1 + hero.x, 3 + hero.y);
					cout << '.';
					map.matrix[hero.y][hero.x] = '.';
					hero.x -= 1;
					isOver = hero.move(map);
				}
			} else if (move == 'd') {
				if (hero.x < map.w - 1) {
					setCursorPosition(1 + hero.x, 3 + hero.y);
					cout << '.';
					map.matrix[hero.y][hero.x] = '.';
					hero.x += 1;
					isOver = hero.move(map);
				}

			} else if (move == 'w') {
				if (hero.y > 0) {
					setCursorPosition(1 + hero.x, 3 + hero.y);
					cout << '.';
					map.matrix[hero.y][hero.x] = '.';
					hero.y -= 1;
					isOver = hero.move(map);
				}

			} else if (move == 's') {
				if (hero.y < map.h - 1) {
					setCursorPosition(1 + hero.x, 3 + hero.y);
					cout << '.';
					map.matrix[hero.y][hero.x] = '.';
					hero.y += 1;
					isOver = hero.move(map);
				}

			} else if (move == 'q') {
				printf("\nScore = %d", hero.money);
				exit(0);
			}

			setCursorPosition(0, 1);
			printf("Money = %d\n\n", hero.money);
			setCursorPosition(1 + hero.x, 3 + hero.y);
#ifndef TARGETOSLINUX
			cout << '*';
#endif

// redraw every frame (use ncurses for cell control)
#ifdef TARGETOSLINUX
      system("clear");
			map.draw();
#endif

			if (map.money == 0) {
				map.init(30, 20);
				do {
					hero.init(map.w, map.h);
				} while (map.matrix[hero.y][hero.x] == '$' || map.matrix[hero.y][hero.x] == '@');
				map.matrix[hero.y][hero.x] = heroSymbol;

				screenClear();
				setCursorPosition(0, 0);
				printf("for move: w - up, s - down, a - left, d - right and ENTER. q - quit\n");
				printf("Money = %d\n", hero.money);
				map.draw();
			}
			fflush(stdout);
		} else {
			setCursorPosition(0, 0);
			cout << "                                                                   ";
			setCursorPosition(0, 0);
			printf("You Lose. Press r for restart or q for quit\n");

      enterRawMode();
			move = getch();
      enterNormalMode();

			if (move == 'r') {
				map.init(30, 20);
				do {
					hero.init(map.w, map.h);
				} while (map.matrix[hero.y][hero.x] == '$' || map.matrix[hero.y][hero.x] == '@');
				map.matrix[hero.y][hero.x] = heroSymbol;
				hero.money = 0;
				isOver = 0;

				screenClear();
				setCursorPosition(0, 0);
				printf("for move: w - up, s - down, a - left, d - right and ENTER. q - quit\n");
				printf("Money = %d\n", hero.money);
				map.draw();

			} else if (move == 'q') {
				exit(0);
			}

		}


	}
}
