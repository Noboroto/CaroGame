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

//DISPLAY CHARACTER
const char DEFAULT_CHAR = '?';
const char BLOCK_CHAR = 'B';
const char UP_CHAR = '^';
const char DOWN_CHAR = 'v';
const char LEFT_CHAR = '<';
const char RIGHT_CHAR = '>';
const char START_CHAR = 'S';
const char FINISH_CHAR = 'F';

int winnerID = -1;
int UserInputInt_ = 0;

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

int main()
{

	return 0;
}