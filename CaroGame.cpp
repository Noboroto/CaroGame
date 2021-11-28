#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <conio.h>  

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

//KEY CHAR
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;

//LIMITATION
const int MAX_ROW = 12;
const int MAX_COL = 12;
const int SPACE_BETWEEN_POINT = 2;
const int NAME_DISPLAY = 7;
const int COL_SIZE = 9;
const int ROW_SIZE = 5;
const int MAX_ACCOUNT = 100;


int winnerID = -1;
int UserInputInt_ = 0;
HANDLE ScreenHandle = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;


struct Player
{
	char Username[NAME_DISPLAY + 1];
	int Win;
	int Tie;
	int Lose;

	Player(int win = 0, int tie = 0, int Lose = 0)
	{
		memset(Username, '\0', NAME_DISPLAY + 1);
		Win = win;
		Tie = tie;
	}
};

Player Accounts_[MAX_ACCOUNT];
int AccountCounter_ = 0;

void loadAccounts()
{
	ifstream file;
	int counter;
	file.open("accounts.txt");
	if (file.fail()) return;
	file >> counter;
	AccountCounter_ = (counter > 100) ? 100 : counter;
	AccountCounter_++;
	for (int i = 1; i < AccountCounter_; ++i)
	{
		Accounts_[i] = Player();
		file >> Accounts_[i].Username;
		file >> Accounts_[i].Win;
		file >> Accounts_[i].Tie;
	}
	file.close();
}

void saveAccounts()
{
	ofstream file;
	file.open("accounts.txt");
	file << AccountCounter_ << '\n';
	for (int i = 1; i < AccountCounter_; ++i)
	{
		file << Accounts_[i].Username << ' ';
		file << Accounts_[i].Win << ' ' << Accounts_[i].Tie << '\n';
	}
	file.close();
}

void moveCursor(int row, int col)
{
	CursorPosition.X = col;
	CursorPosition.Y = row;
	SetConsoleCursorPosition(ScreenHandle, CursorPosition);
}

void ShowCursor(bool CursorVisibility)
{
	CONSOLE_CURSOR_INFO ConCurInf;

	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = CursorVisibility;

	SetConsoleCursorInfo(ScreenHandle, &ConCurInf);
}

struct Point
{
	int DisplayCol;
	int DisplayRow;
	int VerticalLink = 0;
	int HorizontalLink = 0;
	int MainCross = 0;
	int ExtraCross = 0;
	int CurrentPlayer = -1;
	Point(int col = 0, int row = 0)
	{
		DisplayCol = col;
		DisplayRow = row;
	}

	void drawHorizontalLine()
	{
		cout << ' ';
		for (int i = 0; i < COL_SIZE; ++i)
		{
			cout << '-';
		}
		cout << ' ';
	}
	void drawBlankVerticalLin()
	{
		cout << '|';
		for (int i = 0; i < COL_SIZE; ++i)
		{
			cout << ' ';
		}
		cout << '|';
	}
	void drawPoint()
	{
		moveCursor(DisplayRow, DisplayCol);
		drawHorizontalLine();
		moveCursor(DisplayRow + 1, DisplayCol);
		drawBlankVerticalLin();
		moveCursor(DisplayRow + 2, DisplayCol);
		drawBlankVerticalLin();
		moveCursor(DisplayRow + 3, DisplayCol);
		drawBlankVerticalLin();
		moveCursor(DisplayRow + 4, DisplayCol);
		drawHorizontalLine();
	}
};

struct Map
{
	Point Grid[MAX_ROW][MAX_COL];
	char Player[2][NAME_DISPLAY];
	int RowSize = 3;
	int ColSize = 3;
	int WiningMove = 3;
	int TieCounter;

	int CurrentCol = 1;
	int CurrentRow = 1;

	Map(int rowSize = 3, int colsize = 3)
	{
		RowSize = rowSize;
		ColSize = colsize;
		TieCounter = RowSize * colsize;
		int y = 0;
		for (int i = 1; i <= RowSize; ++i)
		{
			Grid[i][1] = Point(0, y);
			for (int j = 2; j <= ColSize; ++j)
			{
				Grid[i][j] = Point(Grid[i][j - 1].DisplayCol + COL_SIZE + 2, y);
			}
			y += 5;
		}
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < NAME_DISPLAY; ++j)
			{
				Player[i][j] = '\0';
			}
		}
	}

	void moveTo(int AddRow, int AddCol)
	{
		int DesRow = CurrentRow + AddRow;
		int DesCol = CurrentCol + AddCol;
		if (DesRow < 1 || DesCol < 1 || DesCol > ColSize || DesRow > RowSize) return;
		moveCursor(Grid[CurrentRow][CurrentCol].DisplayRow + 2, Grid[CurrentRow][CurrentCol].DisplayCol + 1);
		cout << ' ';
		moveCursor(Grid[CurrentRow][CurrentCol].DisplayRow + 2, Grid[CurrentRow][CurrentCol].DisplayCol + COL_SIZE);
		cout << ' ';
		moveCursor(Grid[DesRow][DesCol].DisplayRow + 2, Grid[DesRow][DesCol].DisplayCol + 1);
		cout << '>';
		moveCursor(Grid[DesRow][DesCol].DisplayRow + 2, Grid[DesRow][DesCol].DisplayCol + COL_SIZE);
		cout << '<';
		CurrentCol = DesCol;
		CurrentRow = DesRow;
	}

	bool isWinner(int PlayerOrder)
	{

	}

	void PrintMap()
	{
		ShowCursor(false);
		SMALL_RECT WindowSize;
		WindowSize.Top = 0;
		WindowSize.Left = 0;
		WindowSize.Right = ColSize * (COL_SIZE + 2);
		WindowSize.Bottom = RowSize * ROW_SIZE + 2; 

		COORD NewSize;
		NewSize.X = WindowSize.Right;
		NewSize.Y = WindowSize.Bottom;

		SetConsoleWindowInfo(ScreenHandle, 1, &WindowSize);
		SetConsoleScreenBufferSize(ScreenHandle, NewSize);
		auto hWnd = GetConsoleWindow();
		HMENU hMenu = GetSystemMenu(hWnd, false);

		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
		DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);

		for (int i = 1; i <= RowSize; ++i)
		{
			for (int j = 1; j <= ColSize; ++j)
			{
				Grid[i][j].drawPoint();
			}
		}
		moveTo(0, 0);
	}
};

void navigateToPoint(Map &board)
{
	while (true)
	{
		char c = _getch();
		if (c == ' ' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			switch (c)
			{
			case 'W':
			case 'w':
				board.moveTo(-1,0);
				break;
			case 'S':
			case 's':
				board.moveTo(1, 0);
				break;
			case 'A':
			case 'a':
				board.moveTo(0, -1);
				break;
			case 'D':
			case 'd':
				board.moveTo(0, 1);
				break;
			}
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				board.moveTo(-1, 0);
				break;
			case KEY_DOWN:
				board.moveTo(1, 0);
				break;
			case KEY_LEFT:
				board.moveTo(0, -1);
				break;
			case KEY_RIGHT:
				board.moveTo(0, 1);
				break;
			}
		}
	}
}


int main()
{
	Map test = Map(3, 5);
	test.PrintMap();
	navigateToPoint(test);
	return 0;
}