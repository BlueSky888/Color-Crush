/*
	
	 _______      _                                       _
	( ______)    | |                                     | |
	| |      ___ | | ___   ____     ____  ____ _   _  ___| |__
	| |     / _ \| |/ _ \ / ___)   / ___)/ ___) | | |/___)  _ \
        | |____| |_| | | |_| | |      ( (___| |   | |_| |___ | | | |
 	 \______)___/ \_)___/|_|       \____)_|   |____/(___/|_| |_|

	Jorge García Ferreiro
	English Group - UCM
*/

#include <iostream>
#include <string>
using namespace std;
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <Windows.h>
#include <cctype>
#include <fstream>

const int DIM = 8;
const int ATTEMPTS = 15;
const int COLORNUM = 4;
const int SLEEP = 200;
const bool DEBUG = false;
const string FILENAME = "users.txt";

//////////////////////////////////////////////////////////////////////////
/////////////////// VARIABLES, STRUCTURES AND ARRAYS /////////////////////
//////////////////////////////////////////////////////////////////////////

typedef enum { magenta, yellow, blue, green, none, white_color } tCell;
typedef tCell tBoard[DIM][DIM];
typedef bool tMud[DIM][DIM];

typedef struct
{
	int attempts;
	int points;
	tBoard board;
	tMud mud;
} tGame;

typedef enum { up, down, left_move, right_move, wrong } tDirection;

typedef struct {
	int row;
	int colum;
	tDirection direction;
} tMove;

typedef enum {
	black, dark_blue, dark_green, dark_cyan, dark_red,
	dark_magenta, dark_yellow, light_gray, dark_gray,
	light_blue, light_green, light_cyan, light_red,
	light_magenta, light_yellow, white
} tColor;

typedef bool tArraybool[DIM][DIM];

typedef struct {
	string name;
	int points;
	int played_times;
} tUser;

//////////////////////////////////////////////////////////////////////////
/////////////////////////// PROTOTIPES ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int menu();
void hallOfFame();

// BOARD FUNCTIONS:
void createBoard(tGame &game); // Create a random board each execution time
void initialBoard(tGame &game); // Show a board without tiles formed
void proccessBoard(tGame &game); // Eliminated the tiles formed and erase the mud
void dropTiles(tGame &game, bool &valid); // Fill empty cells and generate new colors

// DISPLAY BOARD
void displayBoard(const tGame &game);
void setColor(tColor color);
void cellToColor(tCell cell, tColor &color);

// MOVEMENT
void readMovement(bool &finish, tMove &move); // Read a valid movement from the keyboard
void processMovement(tGame &game, bool &won); // Proccess the board and checks if the user won
void exchangeTiles(tGame &game, tMove move); // Exchanges tiles
bool checkMove(tGame &game, tMove move); // Check if the user movement is valid
bool checkTiles(const tGame &game, int row, int colum);

// MAIN GAME
void game(int &points, bool &won); 

// EXTRA THINGS
void wonEffect(tGame &game); // Show a "C" in the board when won
void charToDirection(char character, tMove &move);
tCell numberToCell(int num); // Return the number with the tCell type
 
  

int main() {
	srand(time(NULL));
	int option, points;
	bool won;
 
	option = menu();

	while (option != 0) {
		switch (option) {

		case 1: {
			points = 0;
			won = false;
			game(points, won);
		}	break;
		}
		option = menu();
	}


	return 0;
}


int menu() {
	int option;
	bool valid = false;

 	setColor(white);
	cout << "Welcome to Color Crush!" << endl;
	cout << "Are you ready for hours of fun?" << endl << endl;

	setColor(light_cyan);
	cout << "1- Play Hit the Mud" << endl;

	// setColor(light_cyan);
	// cout << "2- Hall of Fame (Under construction)" << endl;

	setColor(light_cyan);
	cout << "0- Exit" << endl;
	cout << endl << "---------" << endl << endl;

	setColor(white);

	do {
		cout << "Choose an option: ";
		cin >> option; 
 
		if ((option < 0) || (option > 1)) {
			valid = false;
			setColor(light_red);
			cout << endl << "-> Invalid option!" << endl << endl;
			setColor(white);
		}
		else {
			valid = true;
		}

	} while (!valid);

	return option;
}

void hallOfFame() {
	ifstream file;
	tUser user;
	string name;
	int num;

	file.open(FILENAME.c_str());

	if (file.is_open()) {

		file >> name;

		while (name != "XXX") {

			cout << "Name: " << name;

			file >> num;
			cout << "Points: " << num;

			file >> num;
			cout << "Points: " << num;

			file >> name;

		}

		file.close();
	}
	else {
		cout << "Sorry. An internal error occurs.";
	}
}

// GAME: Conduct the mud crush and return the points and if the user won

void game(int &points, bool &won) {
	tGame game;
	tMove move;
	int row, colum;
	bool valid = false, initialTiles = true, finish = false;
	won = false;

	createBoard(game);
	initialBoard(game); // Board shown without any tile formed

 	game.attempts = ATTEMPTS;
	game.points = 0;

	while ((!won) && (!finish) && (game.attempts > 0)) {

		displayBoard(game);
		readMovement(finish, move); // Read the move from the user

		if (finish) {
			cout << "You cancel the game! Bye Bye" << endl;
		}
		else {

			valid = checkMove(game, move); // Check the movement
			if ((!finish) && (valid)) {
				game.attempts--;
				processMovement(game, won); // Proccess Board and indicates if the user won
			}
			else if ((!finish) && (!valid)) {
				cout << endl << "Your movement is not valid!" << endl;
			}
		}
	}

	if (won) {
		wonEffect(game);
		cout << setw(21) << right << "Congratulations" << " !!!!!!!!!!!!!!!!" << endl;
		cout << setw(10) << right << "You " << "won the Game!!!!!" << endl;
	}
	else if ((!won) && (!finish)){
		cout << "You don't Win Mud Crush! Sorry no more attemps avaliable" << endl;
	}

	setColor(light_cyan);
	cout << endl << endl << "---------" << endl << endl;
	 
}
 
// Show a board without tiles formed

void initialBoard(tGame &game) {
	bool valid = false, initialTiles = true;
	game.attempts = ATTEMPTS;

	proccessBoard(game);

	do {
		// Show the first game table with no tiles 

		valid = false;

		while (!valid) {
			dropTiles(game, valid);
		}

		proccessBoard(game);
		initialTiles = false;

		for (int colum = 0; colum < DIM; colum++) {
			for (int row = DIM - 1; row >= 0; row--) {

				if (game.board[row][colum] == none) {
					initialTiles = true;
				}

			}
		}

	} while (initialTiles);

}

// Read a valid movement from the keyboard

void readMovement(bool &finish, tMove &move) {
	char character;
	finish = false;

	cout << endl << endl;
	cout << "Row, column and direction letter (U, D, L or R; 0 to end) [Ex: 1 2 U]: ";
	cin >> move.row;

	if (move.row == 0) {
		finish = true;
	}
	else {
		cin >> move.colum;
		cin >> character;

		charToDirection(character, move);

		if (move.colum == 0) {
			finish = true;
		} 		 
	}
}

// WON EFFECT: Show a "C" in the board when won

void wonEffect(tGame &game) {

	displayBoard(game);
	
	for (int row = 0; row < DIM; row++) {
		for (int colum = DIM - 1; colum >= 0; colum--) {

			if ((DIM == 3) || (DIM == 4) || (DIM == 5)) {

				if ((row == DIM - 1) || (row == 0)) {
					game.board[row][colum] = white_color;
				}
				else {

					if ((colum == 0)) {
						game.board[row][colum] = white_color;
					}
					else {
						game.board[row][colum] = none;
					}
				}

			}
			else {

				if ((row == DIM - 1) || (row == DIM - 2) || (row == 0) || (row == 1)) {
					game.board[row][colum] = white_color;
				}
				else {

					if ((colum == 0) || (colum == 1)) {
						game.board[row][colum] = white_color;
					}
					else {
						game.board[row][colum] = none;
					}

				}
			}
		}
	}
	displayBoard(game);
}

// Proccess the board and checks if the user won

void processMovement(tGame &game, bool &won) {
	bool valid = false, initialTiles = true;

	proccessBoard(game);
	displayBoard(game);

	do {

		valid = false;

		while (!valid) {
			dropTiles(game, valid);
			displayBoard(game);
		}

		proccessBoard(game);
		displayBoard(game);
		initialTiles = false;

		for (int colum = 0; colum < DIM; colum++) {
			for (int row = DIM - 1; row >= 0; row--) {

				if (game.board[row][colum] == none) {
					initialTiles = true;
				}

			}
		}

	} while (initialTiles);

	won = true; // Checks if the user won

	for (int colum = 0; colum < DIM; colum++) {
		for (int row = DIM - 1; row >= 0; row--) {

			if (game.mud[row][colum] == true) {
				won = false;
			}

		}
	}

}

// Function for coloring the characters

void setColor(tColor color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
}

// Create randomly  a multidimensional array with n DIMENSION

void createBoard(tGame &game) {
	int randNum = 0;
	tCell cell;

	for (int colum = 0; colum < DIM; colum++) {
		for (int row = DIM - 1; row >= 0; row--) {
			game.mud[row][colum] = true;
		}
	}

	for (int colum = 0; colum < DIM; colum++) {
		for (int row = DIM - 1; row >= 0; row--) {
			randNum = rand() % COLORNUM;  // Generate a number between 0 and 3
			cell = numberToCell(randNum); // Convert the number into tCell
			game.board[row][colum] = cell; // Save the number into the array
		}
	}

}

// Convert a number into the Cell type

tCell numberToCell(int num) {
	tCell cell;

	if (num == 0) {
		cell = magenta; // Magenta Color
	}
	else if (num == 1) {
		cell = yellow; // Yello Color
	}
	else if (num == 2) {
		cell = blue; // Blue Color
	}
	else if (num == 3) {
		cell = green; // Green Color
	}
	else {
		cell = none; // No Color
	}
	return cell;
}

// Display the table of the board, with its cells colouring

void displayBoard(const tGame &game) {
	tColor color;
	setColor(white);

	// DEBUG NOT ACTIVED
	if (!DEBUG) {
		system("cls");
	}

	cout << endl << setw(19) << right << "Attempts: " << game.attempts;
	cout << "  Points: " << game.points;
	cout << endl << endl;

	// TOP PART OF THE TABLE
	// Let's create the top part of the table. Before, it's necessary a setw for the table design 

	cout << setw(6);

	for (int top_row = 0; top_row < DIM; top_row++) {

		if (top_row == 0) {
			cout << char(218); // Top left corner
		}
		if (top_row < DIM - 1) {
			cout << char(196) << char(196) << char(196) << char(194); // cells design
		}
		else {
			cout << char(196) << char(196) << char(196) << char(191); // Last cell, little design variation in the last char (with corner)
			cout << endl;
		}
	}

	// CELLS TABLE STRUCTURE
	// Basically the program creates a for loop with each row and colum. The show the cells with an specific color 

	for (int row = DIM - 1; row >= 0; row--) {

		cout << setw(3) << right << row + 1 << setw(3);  // Print the row number

		for (int colum = 0; colum < DIM; colum++) {
			setColor(white);
			cout << char(179); // Sidebar
			cellToColor(game.board[row][colum], color);

			if (game.mud[row][colum] == true) {
				setColor(white); // Change the color of the cells
				cout << char(219);
				setColor(color); // Change the color of the cells
				cout << char(219);
				setColor(white); // Change the color of the cells
				cout << char(219);
			}
			else {

				setColor(color); // Change the color of the cells
				cout << char(219) << char(219) << char(219); // CELLS
			}

			if (colum == DIM - 1) {
				setColor(white); // Restore the color into white
				cout << char(179); // Last cell, little design variation in the last char (with corner)
			}

		}

		// SEPARATED LINES
		// It print on the screen the lines for separating each row. It doesn't show a line in the last row 

		if (row > 0) {

			cout << endl << setw(6) << char(195);

			for (int colum = 0; colum < DIM; colum++) {
				if (colum < DIM - 1) {
					cout << char(196) << char(196) << char(196) << char(197); // Botton lines
				}
				else {
					cout << char(196) << char(196) << char(196) << char(180) << endl;  // Bottom right corner
				}
			}
		}

	}

	// BOTTOM TABLE DESIGN
	// The same design as the top, but changing some characters with the appropiate ones 

	cout << endl << setw(6);

	for (int bottom_row = 0; bottom_row < DIM; bottom_row++) {

		if (bottom_row == 0) {
			cout << char(192);
		}
		if (bottom_row < DIM - 1) {
			cout << char(196) << char(196) << char(196) << char(193); // Lines
		}
		else {
			cout << char(196) << char(196) << char(196) << char(217); // Bottom right corner
		}
	}

	cout << endl << setw(8);

	for (int i = 0; i < DIM; i++) {
		cout << right << i + 1 << setw(4); // Print the colums number
	}

	// DEBUG NOT ACTIVED

	if (!DEBUG) {
		Sleep(SLEEP);
	}
	else {
		cout << endl << endl;
		system("pause");
	}

	cout << endl << endl;

}

void cellToColor(tCell cell, tColor &color) {

	if (cell == magenta) {
		color = light_magenta;
	}
	else if (cell == yellow) {
		color = light_yellow;
	}
	else if (cell == blue) {
		color = light_blue;
	}
	else if (cell == green) {
		color = light_green;
	}
	else if (cell == none) {
		color = black;
	}
	else {
		color = white;
	}
}

// To check if a user move is valid (row, column and direction are valid, destination is inside the board and at least one tile group is formed).

bool checkMove(tGame &game, tMove move) {
	bool valid = true, tileFormed = false;

	int row = move.row;
	int colum = move.colum;

	if (((row < 1) || (row > DIM)) || ((colum < 1) || (colum > DIM))) {
		cout << "Your row or colum is not valid!";
		valid = false; // Row or colum greater or lesser than DIM or 0
	}
	else if ((row == DIM) && (move.direction == up)) {
		cout << "Invalid movement. You can't go up there!";
		valid = false; // Wrong: First row and direction is up
	}
	else if ((row == 1) && (move.direction == down)) {
		cout << "Invalid movement. You can't go down there!";
		valid = false; // Wrong: Last row and direction is down
	}
	else if ((colum == 1) && (move.direction == left_move)) {
		cout << "Invalid movement. You can't go left there!";
		valid = false; // Wrong: First colum and direction is left
	}
	else if ((colum == DIM) && (move.direction == right_move)) {
		cout << "Invalid movement. You can't go Right there!";
		valid = false;  // Wrong: Last colum and direction is Right
	}
	else if (move.direction == wrong) { // Wrong if the direction is not valid
		cout << endl << "Invalid movement. Your direction is not recognised";
		valid = false;
	}

	cout << endl;

	if (valid) {

		// First: Exchange the tiles, depending on the movement
		exchangeTiles(game, move);

		// Second: We check if there's a tile formed
		tileFormed = checkTiles(game, row, colum);

		if (tileFormed) {
			valid = true; // The movement is correct
		}
		else {

			// In case there is not a row or colum formed in this position. Change the value of the Row and colum 

			if (move.direction == up) {
				row++;
			}
			else if (move.direction == down) {
				row--;
			}
			else if (move.direction == left_move) {
				colum--;
			}
			else if (move.direction == right_move) {
				colum++;
			}

			valid = checkTiles(game, row, colum); // Let's check again. But with the other row and colum

		}

		if (!valid) {
			// Not valid? Undo the tiles exchanges.
			exchangeTiles(game, move);
		}
	}

	return valid;
}

// Echange Two tiles depending on the movement

void exchangeTiles(tGame &game, tMove move) {
	tCell cell;
	int row = move.row - 1;
	int colum = move.colum - 1;

	cell = game.board[row][colum];

	if (move.direction == up) {
		game.board[row][colum] = game.board[row + 1][colum];
		game.board[row + 1][colum] = cell;
	}
	else if (move.direction == down) {
		game.board[row][colum] = game.board[row - 1][colum];
		game.board[row - 1][colum] = cell;
	}
	else if (move.direction == left_move) {
		game.board[row][colum] = game.board[row][colum - 1];
		game.board[row][colum - 1] = cell;
	}
	else if (move.direction == right_move) {
		game.board[row][colum] = game.board[row][colum + 1];
		game.board[row][colum + 1] = cell;
	}
}

// CHECK TILES
// To check if there are any tile groups involving a specific board position, either horizontally or vertically.

bool checkTiles(const tGame &game, int row, int colum) {
	bool valid = false;
	int auxRow, auxColum, count, i;
	tCell cell;

	row--;
	colum--;
	auxRow = row;
	auxColum = colum;


	count = 1;
	cell = game.board[auxRow][auxColum];

	auxColum--;

	while ((auxColum >= 0) && (game.board[auxRow][auxColum] == cell)) {
		auxColum--;
		count++;
	}

	if (count < 3)  {

		auxColum = colum + 1;
		while ((auxColum < DIM) && (game.board[auxRow][auxColum] == cell)) {
			auxColum++;
			count++;
		}

	}

	auxRow = row;
	auxColum = colum;

	if (count < 3) {
		count = 1;
		auxRow = row + 1;
		auxColum = colum;

		while ((auxRow < DIM) && (game.board[auxRow][auxColum] == cell)) {
			auxRow++;
			count++;
		}

		if (count < 3) {
			auxRow = row - 1;
			while ((auxRow >= 0) && (game.board[auxRow][auxColum] == cell)) {
				auxRow--;
				count++;
			}
		}

	}

	if (count >= 3) {
		valid = true;
	}
	else {
		valid = false;
	}


	return valid;
}

// PROCESS BOARD: Eliminated the tiles formed and erase the mud

void proccessBoard(tGame &game) {
	int row = 0, colum = 0, counter;
	tCell cell;
	tArraybool boolTable;

	for (int row = DIM - 1; row >= 0; row--) {
		for (int colum = 0; colum < DIM; colum++) {
			boolTable[row][colum] = false; // Initialing the Bool table to False.
		}
	}

	/***************************************/
	/******  COMPROBACIÓN HORIZONTAL  ******/
	/***************************************/

	for (int row = DIM - 1; row >= 0; row--) {

		counter = 1;
		cell = none;

		for (int colum = 0; colum < DIM; colum++) {

			if (game.board[row][colum] == cell) {
				counter++;
			}
			else {
				counter = 1;
				cell = game.board[row][colum];
			}

			if (counter >= 3)
			{
				for (int i = 0; i < counter; i++) {
					boolTable[row][colum - i] = true; // Bool Taable to false
				}
				if (counter == 3) {
					game.points = game.points + 3; // 3 points
				}
				else if (counter == 4) {
					game.points = game.points + 2; // 5 points
				}
				else if (counter == 5) {
					game.points = game.points + 2; // 7 points
				}
			}
		}

	}
	 
	/***************************************/
	/*******  COMPROBACIÓN VERTICAL  *******/
	/***************************************/

	for (int colum = DIM - 1; colum >= 0; colum--) {

		counter = 1;
		cell = none;

		for (int row = 0; row < DIM; row++) {

			if (game.board[row][colum] == cell) {
				counter++;
			}
			else {
				counter = 1;
				cell = game.board[row][colum];
			}

			if (counter >= 3)
			{
				for (int i = 0; i < counter; i++) {
					boolTable[row - i][colum] = true; // Bool Taable to false
				}
				if (counter == 3) {
					game.points = game.points + 3; // 3 points
				}
				else if (counter == 4) {
					game.points = game.points + 2; // 5 points
				}
				else if (counter == 5) {
					game.points = game.points + 2; // 7 points
				}
			}
		}

	}

	for (int colum = 0; colum < DIM; colum++) {
		for (int row = DIM - 1; row >= 0; row--) {

			if ((game.attempts == ATTEMPTS) && (boolTable[row][colum] == true)) {
				game.board[row][colum] = none;
			}

			if ((game.attempts < ATTEMPTS) && (boolTable[row][colum] == true)) {
				game.mud[row][colum] = false;
				game.board[row][colum] = none;
			}

		}
	}

}

// DROP TILES: Fill empty cells and generate new colors

void dropTiles(tGame &game, bool &valid) {
	tCell cell;
	int randNum, row, colum;
	valid = false;

	for (int colum = 0; colum < DIM; colum++) {
		for (int row = 0; row < DIM; row++) {

			if ((game.board[row][colum] == none) && (row == DIM - 1)) {
				randNum = rand() % COLORNUM;  // Generate a number between 0 and 3
				cell = numberToCell(randNum); // Convert the number into tCell
				game.board[row][colum] = cell;
			}
			else if (game.board[row][colum] == none) {
				game.board[row][colum] = game.board[row + 1][colum];
				game.board[row + 1][colum] = none;
			}

		}
	}

	valid = true;

	// Checks if the board is correct, and without empty cells
	for (int row = 0; row < DIM; row++) {
		for (int colum = 0; colum < DIM; colum++) {
			if (game.board[row][colum] == none) {
				valid = false;
			}
		}
	}

}

// Char to direction

void charToDirection(char character, tMove &move) {

	character = toupper(character);

	if (character == 'U') {
		move.direction = up;
	}
	else if (character == 'D') {
		move.direction = down;
	}
	else if (character == 'L') {
		move.direction = left_move;
	}
	else if (character == 'R') {
		move.direction = right_move;
	}
	else {
		move.direction = wrong;
	}


}
