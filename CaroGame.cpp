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
const int NAME_DISPLAY = 8;
const int COL_SIZE = 9;
const int ROW_SIZE = 5;
const int MAX_ACCOUNT = 100;
const int NOTICE_SIZE = 1;
const int NOTICE_COL = (COL_SIZE + 2) * (MAX_COL - 2) + 1;
const int WINDOWS_WIDTH = (COL_SIZE + 2) * (MAX_COL - 2) + NAME_DISPLAY - 1 + NOTICE_SIZE;
const int WINDOWS_HEGHT = ROW_SIZE * (MAX_ROW - 2) + 2;


int winnerID = -1;
int UserInputInt_ = 0;
bool isSingleMode = false;
HANDLE OutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE InHamdle = GetStdHandle(STD_INPUT_HANDLE);
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
	SetConsoleCursorPosition(OutHandle, CursorPosition);
}

void showCursor(bool CursorVisibility)
{
	CONSOLE_CURSOR_INFO ConCurInf;

	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = CursorVisibility;

	SetConsoleCursorInfo(OutHandle, &ConCurInf);
}

void changeWindows(int height, int width)
{
	SMALL_RECT WindowSize;
	WindowSize.Top = 0;
	WindowSize.Left = 0;
	WindowSize.Right = width;
	WindowSize.Bottom = height;

	COORD NewSize;
	NewSize.X = WindowSize.Right;
	NewSize.Y = WindowSize.Bottom;

	//change windows size
	SetConsoleWindowInfo(OutHandle, 1, &WindowSize);
	//change buffer size
	SetConsoleScreenBufferSize(OutHandle, NewSize);

	auto hWnd = GetConsoleWindow();
	HMENU hMenu = GetSystemMenu(hWnd, false);

	//disable changing windows and buffer size
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
	DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	ShowScrollBar(hWnd, SB_BOTH, false);
}

void inputCharArray(char c[],const int max)
{
	char x;
	int i = 0;
	do
	{
		x = cin.get();
		if (x == '\n' || x == '\r' || x == '\0') break;
		if (i < max - 1)
		{
			c[i] = x;
			i++;
		}
	} while (x != '\n' && x != '\r' && x != '\0');
	c[i] = '\0';
}

int inputInteger()
{
	char c;
	int res = 0;
	do
	{
		c = _getch();
		if (c >= '0' && c <= '9')
		{
			res = res * 10 + (c - '0');
			cout << c;
		}
		else if (c == '\b')
		{
			res /= 10;
			cout << c;
			cout << ' ';
			cout << c;
		}
	} while (c != '\n' && c != '\r');
	cout << '\n';
	return res;
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
	Point(int row = 0, int col = 0)
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
	int WiningCounter = 3;
	int TurnCounter = 0;
	int TieCounter;

	int CurrentCol = 1;
	int CurrentRow = 1;

	int startScreenCol(int colsize = 10)
	{
		return (WINDOWS_WIDTH - NAME_DISPLAY - 1 - NOTICE_SIZE - ((colsize + 2) * COL_SIZE)) / 2;
	}

	Map()
	{
		int rowsize = 0, colsize = 0;
		get_col:
			cout << "How many column you want to use? (from 3 to 10) ";
			colsize = inputInteger();
			if (colsize < 3 || colsize > 10)
			{
				cout << "Out of range or wrong format! Please try again!\n";
				goto get_col;
			}
		get_row:
			cout << "How many row you want to use? (from 3 to 10) ";
			rowsize = inputInteger();
			if (rowsize < 3 || rowsize > 10)
			{
				cout << "Out of range or wrong format! Please try again!\n";
				goto get_row;
			}
		RowSize = rowsize;
		ColSize = colsize;
		TieCounter = RowSize * colsize;
		int y = 0;
		int startX = startScreenCol(colsize);
		for (int i = 1; i <= RowSize; ++i)
		{
			Grid[i][1] = Point(y, startX);
			for (int j = 2; j <= ColSize; ++j)
			{
				Grid[i][j] = Point(y, Grid[i][j - 1].DisplayCol + COL_SIZE + 2);
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

	void getWiningCounter()
	{
		if (min(ColSize, RowSize) == WiningCounter) return;
		wining_move:
		cout << "How many continuous point to win? (from 3 to " << min(ColSize, RowSize) << ") ";
		int selection = inputInteger();
		if (selection < WiningCounter || selection > min(ColSize, RowSize))
		{
			cout << "Out of range or wrong format! Please try again!\n";
			goto wining_move;
		}
		WiningCounter = selection;
	}

	void getPlayerInfo(int id)
	{
		cout << "Please type player " << (id + 1) << " symbol (max: 7 characters) ";
		showCursor(true);
		inputCharArray(Player[id], NAME_DISPLAY);
		int n = strlen(Player[id]);
		if (n < NAME_DISPLAY - 1)
		{
			char tmp[NAME_DISPLAY];
			strcpy_s(tmp,Player[id]);
			for (int i = 0; i < NAME_DISPLAY - 1; ++i)
			{
				Player[id][i] = ' ';
			}
			Player[id][(NAME_DISPLAY - n) / 2] = '\0';
			strcat_s(Player[id], tmp);
		}
	}

	bool isWinner(int PlayerOrder)
	{

	}

	void printNotice()
	{
		moveCursor(0, NOTICE_COL);
		cout << "         ";
		moveCursor(0, NOTICE_COL);
		cout << "Turn " << TurnCounter + 1;
		moveCursor(1, NOTICE_COL);
		cout << "         ";
		moveCursor(1, NOTICE_COL);
		for (int i = 0; i < strlen(Player[TurnCounter % 2]); ++i)
		{
			if (Player[TurnCounter % 2][i] != ' ') cout << Player[TurnCounter % 2][i];
		}
	}

	void printMap()
	{
		system("cls");
		showCursor(false);

		for (int i = 1; i <= RowSize; ++i)
		{
			for (int j = 1; j <= ColSize; ++j)
			{
				Grid[i][j].drawPoint();
			}
		}
		printNotice();
		moveTo(0, 0);
	}

	void selectPoint()
	{
		int playerid = TurnCounter % 2;
		if (Grid[CurrentRow][CurrentCol].CurrentPlayer != -1) return;
		moveCursor(Grid[CurrentRow][CurrentCol].DisplayRow + 2, Grid[CurrentRow][CurrentCol].DisplayCol + 2);
		cout << Player[playerid];
		Grid[CurrentRow][CurrentCol].CurrentPlayer = playerid;
		TurnCounter++;
		TieCounter--;
		printNotice();
	}

	void navigateToPoint()
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
					moveTo(-1, 0);
					break;
				case 'S':
				case 's':
					moveTo(1, 0);
					break;
				case 'A':
				case 'a':
					moveTo(0, -1);
					break;
				case 'D':
				case 'd':
					moveTo(0, 1);
					break;
				}
			}
			else if (c == '\n' || c == '\r')
			{
				selectPoint();
			}
			else
			{
				c = _getch();
				switch (c)
				{
				case KEY_UP:
					moveTo(-1, 0);
					break;
				case KEY_DOWN:
					moveTo(1, 0);
					break;
				case KEY_LEFT:
					moveTo(0, -1);
					break;
				case KEY_RIGHT:
					moveTo(0, 1);
					break;
				}
			}
		}
	}
};

int main()
{
	//Disable selection
	SetConsoleMode(InHamdle, ~ENABLE_QUICK_EDIT_MODE);

	//change windows size
	changeWindows(WINDOWS_HEGHT, WINDOWS_WIDTH);
	
	Started:
	system("cls");	
	Map test = Map();
	test.getWiningCounter();
	test.getPlayerInfo(0);
	test.getPlayerInfo(1);
	test.printMap();
	test.navigateToPoint();
	return 0;
}