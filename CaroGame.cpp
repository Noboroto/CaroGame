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
const int MAX_PLAYER_PER_MAP = 4;
const int MAX_HORSE_PER_PLAYER = 4;
const int MAX_ROW = 10;
const int MAX_COL = 10;
const int SPACE_BETWEEN_POINT = 2;
const int MAX_NAME = 7;
const int MAX_CHAR_DISPLAY = 9;
const int MID_NAME_DISPLAY = MAX_CHAR_DISPLAY / 2;
const int MAX_ACCOUNT = 100;


int winnerID = -1;
int UserInputInt_ = 0;
HANDLE ScreenHandle = GetStdHandle(STD_OUTPUT_HANDLE);

struct Player
{
	char Username[MAX_NAME + 1];
	int Win;
	int Tie;
	int Lose;

	Player(int win = 0, int tie = 0, int Lose = 0)
	{
		memset(Username, '\0', MAX_NAME + 1);
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

struct Point
{
	int Col;
	int Row;
	int VerticalLink = 0;
	int HorizontalLink = 0;
	int MainCross = 0;
	int ExtraCross = 0;
	Point(int col = 0, int row = 0)
	{
		Col = col;
		Row = row;
	}

	void drawHorizontalLine()
	{
		cout << ' ';
		const int DISPLAY_SIZE = MAX_CHAR_DISPLAY;
		for (int i = 0; i < DISPLAY_SIZE; ++i)
		{
			cout << '-';
		}
		cout << ' ';
	}
	void drawBlankVerticalLin()
	{
		cout << '|';
		for (int i = 0; i < MAX_CHAR_DISPLAY; ++i)
		{
			cout << ' ';
		}
		cout << '|';
	}
	void drawPoint()
	{
		COORD Position;
		Position.X = Col;
		Position.Y = Row;
		SetConsoleCursorPosition(ScreenHandle, Position);
		drawHorizontalLine();
		Position.Y++;
		SetConsoleCursorPosition(ScreenHandle, Position);
		drawBlankVerticalLin();
		Position.Y++;
		SetConsoleCursorPosition(ScreenHandle, Position);
		drawBlankVerticalLin();
		Position.Y++;
		SetConsoleCursorPosition(ScreenHandle, Position);
		drawBlankVerticalLin();
		Position.Y++;
		SetConsoleCursorPosition(ScreenHandle, Position);
		drawHorizontalLine();
	}
};


int main()
{
	Point abc = Point(5, 5);
	abc.drawPoint();
	Point xyz = Point(5, 10);
	xyz.drawPoint();
	return 0;
}