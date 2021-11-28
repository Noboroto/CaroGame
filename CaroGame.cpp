#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

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

struct Point
{
	int Col;
	int Row;
	int VerticalLink = 0;
	int HorizontalLink = 0;
	int MainCross = 0;
	int ExtraCross = 0;
	int CurrentPlayer = -1;
	Point(int col = 0, int row = 0)
	{
		Col = col;
		Row = row;
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
		moveCursor(Row, Col);
		drawHorizontalLine();
		moveCursor(Row + 1, Col);
		drawBlankVerticalLin();
		moveCursor(Row + 2, Col);
		drawBlankVerticalLin();
		moveCursor(Row + 3, Col);
		drawBlankVerticalLin();
		moveCursor(Row + 4, Col);
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
				Grid[i][j] = Point(Grid[i][j - 1].Col + COL_SIZE + 2, y);
			}
			y += 5;
		}
	}

	bool isWinner(int PlayerOrder)
	{

	}

	void PrintMap()
	{
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
	}
};


int main()
{
	Map test = Map(3, 7);
	test.PrintMap();
	char x;
	while (true)
	{
		cin >> x;
	}
	return 0;
}