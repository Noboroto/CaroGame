#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstring>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <mmsystem.h>

using std::cin;
using std::cout;
using std::ifstream;
using std::min;
using std::ofstream;

//COLOR
const int BLACK = 0;
const int WHITE = 7;
const int RED = 4;
const int GRAY = 8;
const int GREEN = 2;
const int YELLOW = 6;
const int MAX_COLOR_CODE = 15;

//KEY CHAR
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;

//LIMITATION
const int MAX_PAS = 20;
const int MAX_ROW = 14;
const int MAX_COL = 20;
const int SPACE_BETWEEN_POINT = 2;
const int NAME_DISPLAY = 8;
const int COL_SIZE = 9;
const int ROW_SIZE = 5;
const int MAX_ACCOUNT = 100;
const int NOTICE_SIZE = 8;
const int WINDOWS_WIDTH = (COL_SIZE + 2) * (MAX_COL - 2) + NAME_DISPLAY - 1 + 1;
const int WINDOWS_HEGHT = ROW_SIZE * (MAX_ROW - 2) + 2;

bool isSingleMode = false;
HANDLE OutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE InHamdle = GetStdHandle(STD_INPUT_HANDLE);
COORD CursorPosition;
int Notice_Col = 0;

struct Coordinate
{
	int Row = 0, Col = 0, Turn = 0;
	Coordinate(int row = 0, int col = 0, int t = 0)
	{
		Row = row;
		Col = col;
		Turn = t;
	}
};

struct CoordinateStack
{
	Coordinate Root[MAX_COL * MAX_ROW];
	int Size = 0;
	void push(Coordinate item)
	{
		Root[Size] = item;
		Size++;
	}
	Coordinate pop()
	{
		Size--;
		return (Root[Size]);
	}
	void clear()
	{
		Size = 0;
	}
	bool empty()
	{
		return !Size;
	}
};

struct Player
{
	char Username[NAME_DISPLAY + 1];
	char Password[MAX_PAS + 1];
	int Win;
	int Tie;
	int Lose;

	Player(int win = 0, int tie = 0, int lose = 0)
	{
		memset(Username, '\0', NAME_DISPLAY + 1);
		memset(Password, '\0', MAX_PAS + 1);
		Win = win;
		Tie = tie;
		Lose = lose;
	}

	void setName(char name[])
	{
		strcpy_s(Username, name);
	}
	void setPassword(char pass[])
	{
		strcpy_s(Password, pass);
	}
	bool checkPassword(char pass[])
	{
		return !strcmp(Password, pass);
	}
};

Player Accounts_[MAX_ACCOUNT + 1];
int AccountCounter_ = 0;
int ActiveID = -1;

void loadAccounts()
{
	ifstream file;
	int counter;
	file.open("accounts.etai");
	if (file.fail())
		return;
	file >> counter;
	AccountCounter_ = (counter > MAX_ACCOUNT) ? MAX_ACCOUNT : counter;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		Accounts_[i] = Player();
		file >> Accounts_[i].Username;
		file >> Accounts_[i].Password;
		file >> Accounts_[i].Win;
		file >> Accounts_[i].Tie;
		file >> Accounts_[i].Lose;
	}
	file.close();
}

void saveAccounts()
{
	ofstream file;
	file.open("accounts.etai");
	file << AccountCounter_ << '\n';
	for (int i = 0; i < AccountCounter_; ++i)
	{
		file << Accounts_[i].Username << '\n';
		file << Accounts_[i].Password << '\n';
		file << Accounts_[i].Win << ' ' << Accounts_[i].Tie << ' ' << Accounts_[i].Lose << '\n';
	}
	file.close();
}

void removeAccount(char name[])
{
	int id = -1;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (!strcmp(name, Accounts_[i].Username))
		{
			id = i;
			break;
		}
	}
	AccountCounter_--;
	for (int i = id; i < AccountCounter_; ++i)
	{
		Accounts_[i] = Accounts_[i + 1];
	}
	saveAccounts();
}

int registerAccount(char name[], char pass[])
{
	if (strlen(name) <= 0) return -2;
	if (!strcmp(name, "BOT")) return 0;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (!strcmp(name, Accounts_[i].Username))
			return 0;
	}
	Accounts_[AccountCounter_].setName(name);
	Accounts_[AccountCounter_].setPassword(pass);
	ActiveID = AccountCounter_;
	AccountCounter_++;
	saveAccounts();
	return 1;
}

int loginAccount(char name[], char pass[])
{
	if (strlen(name) <= 0) return -2;
	for (int i = 0; i < AccountCounter_; ++i)
	{
		if (!strcmp(name, Accounts_[i].Username))
		{
			if (Accounts_[i].checkPassword(pass))
			{
				ActiveID = i;
				return 0;
			}
			else
				return 1;
		}
	}
	return -1;
}

void setColor(int backgound_color, int text_color)
{
	int color_code = backgound_color * 16 + text_color;
	SetConsoleTextAttribute(OutHandle, color_code);
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

int getRandomNumber(int from, int to)
{
	srand(time(NULL));
	return (rand() % to) + from;
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

	//change buffer size
	SetConsoleScreenBufferSize(OutHandle, NewSize);
	WindowSize.Right = width - 1;
	WindowSize.Bottom = height - 1;
	//change windows size
	SetConsoleWindowInfo(OutHandle, 1, &WindowSize);

	auto hWnd = GetConsoleWindow();
	HMENU hMenu = GetSystemMenu(hWnd, false);

	//disable changing windows and buffer size
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
	DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	ShowScrollBar(hWnd, SB_BOTH, false);
}

void inputCharArray(char c[], const int max, char hide = '\0')
{
	char x;
	int i = strlen(c);
	if (hide == '\0')
		cout << c;
	else
	{
		for (int j = 0; j < i; ++j)
			cout << hide;
	}
	showCursor(true);
	do
	{
		x = _getch();
		if (x == '\n' || x == '\r' || x == '\0')
		{
			if (i <= 0) continue;
			cout << '\n';
			break;
		}
		if (x == '\b' && i > 0)
		{
			cout << x;
			cout << ' ';
			cout << x;
			i--;
		}
		else if (x != '\b' && i < max - 1)
		{
			if (hide == '\0')
				cout << x;
			else
				cout << hide;
			c[i] = x;
			i++;
		}
	} while ((x != '\n' && x != '\r' && x != '\0') || i <= 0);
	if (i > 0)
		c[i] = '\0';
	showCursor(false);
}

int inputPositiveInteger(int from, int to)
{
	char c;
	int max_char = 0, tmp = to;
	while (tmp > 0)
	{
		max_char++;
		tmp /= 10;
		cout << ' ';
	}
	for (int i = 0; i < max_char; ++i)
	{
		cout << '\b';
	}
	int i = 0;
	int res = 0;
	do
	{
		c = _getch();
		if (c >= '0' && c <= '9')
		{
			if (i >= max_char)
				continue;
			if ((res * 10 + (c - '0') <= to))
			{
				res = res * 10 + (c - '0');
				cout << c;
				i++;
			}
		}
		else if (c == '\b' && i > 0)
		{
			res /= 10;
			cout << c;
			cout << ' ';
			cout << c;
			i--;
		}
	} while ((c != '\n' && c != '\r') || res < from);
	cout << '\n';
	return res;
}

struct Point
{
	int DisplayCol;
	int DisplayRow;
	int CurrentPlayer = -1;
	int TopLeft = 0, Top = 0, TopRight = 0, Right = 0, BottomRight = 0, Bottom = 0, BottomLeft = 0, Left = 0;

	void Initialize()
	{
		TopLeft = 0, Top = 0, TopRight = 0, Right = 0, BottomRight = 0, Bottom = 0, BottomLeft = 0, Left = 0;
		CurrentPlayer = -1;
	}

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
	CoordinateStack MoveOfAll;
	Coordinate Suggestion;
	Point Grid[MAX_ROW][MAX_COL];
	char Player[2][NAME_DISPLAY];
	int PlayerColor[2];
	int RowSize = 3;
	int ColSize = 3;
	int WiningCounter = 3;
	int TurnCounter = 0;
	int WinnerID = -1;
	bool UseBot = false;
	bool BackgroundSound = false;
	int TurnRestriction = 0;

	int CurrentCol = 1;
	int CurrentRow = 1;

	void Initialize()
	{
		int y = 0;
		int startX = 0;
		WinnerID = -1;
		TurnCounter = 0;
		CurrentRow = 1;
		CurrentCol = 1;
		MoveOfAll.clear();
		Suggestion = Coordinate(-1, -1);
		for (int i = 1; i <= RowSize; ++i)
		{
			Grid[i][1] = Point(y, startX);
			for (int j = 2; j <= ColSize; ++j)
			{
				Grid[i][j] = Point(y, Grid[i][j - 1].DisplayCol + COL_SIZE + 2);
			}
			y += 5;
		}
	}

	Map()
	{
		RowSize = 3;
		ColSize = 3;
		WiningCounter = 3;
		TurnCounter = 0;
		WinnerID = -1;
		UseBot = false;
		BackgroundSound = false;
		TurnRestriction = 0;

		CurrentCol = 1;
		CurrentRow = 1;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < NAME_DISPLAY; ++j)
			{
				Player[i][j] = '\0';
			}
		}
		Player[0][0] = 'X';
		Player[1][0] = 'O';
		PlayerColor[0] = 4, PlayerColor[1] = 10;
	}

	void moveTo(int AddRow, int AddCol)
	{
		int DesRow = CurrentRow + AddRow;
		int DesCol = CurrentCol + AddCol;
		if (DesRow < 1 || DesCol < 1 || DesCol > ColSize || DesRow > RowSize)
			return;
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
		if (min(ColSize, RowSize) == WiningCounter)
			return;
		cout << "How many continuous point to win? (from 3 to " << min(ColSize, RowSize) << ") ";
		int selection = inputPositiveInteger(WiningCounter, min(ColSize, RowSize));
		WiningCounter = selection;
	}

	void rewriteName(int id)
	{
		int n = strlen(Player[id]);
		if (n < NAME_DISPLAY - 1)
		{
			char tmp[NAME_DISPLAY];
			strcpy_s(tmp, Player[id]);
			for (int i = 0; i < NAME_DISPLAY - 1; ++i)
			{
				Player[id][i] = ' ';
			}
			Player[id][(NAME_DISPLAY - n) / 2] = '\0';
			strcat_s(Player[id], tmp);
		}
	}

	void printNotice()
	{
		int row = 0;
		moveCursor(row, Notice_Col);
		cout << "         ";
		moveCursor(row, Notice_Col);
		cout << "Turn " << TurnCounter + 1;
		row++;
		moveCursor(row, Notice_Col);
		cout << "         ";
		moveCursor(row, Notice_Col);
		setColor(BLACK, PlayerColor[TurnCounter % 2]);
		for (int i = 0; i < strlen(Player[TurnCounter % 2]); ++i)
		{
			if (Player[TurnCounter % 2][i] != ' ')
				cout << Player[TurnCounter % 2][i];
		}
		if (TurnRestriction != 0)
		{
			setColor(YELLOW, BLACK);
			row++;
			moveCursor(row, Notice_Col);
			cout << "After " << std::max(TurnRestriction - TurnCounter, 0) << " the game will end!";
			setColor(BLACK, WHITE);
		}
		row++;
		setColor(BLACK, WHITE);
		moveCursor(row, Notice_Col);
		cout << "Press key to: ";
		row++;
		moveCursor(row, Notice_Col);
		cout << "[M] Move Suggestion";
		row++;
		moveCursor(row, Notice_Col);
		cout << "[U] Undo";
		row++;
		moveCursor(row, Notice_Col);
		cout << "[R] Replay";
		row++;
		moveCursor(row, Notice_Col);
		cout << "[Q] Return to Main Menu";
		row++;
		moveCursor(row, Notice_Col);
		cout << "[E] Exit game";
	}

	void printMap()
	{
		system("cls");
		showCursor(false);
		int WindowsRow = ROW_SIZE * RowSize + 4;
		int WindowsCol = (COL_SIZE + 2) * ColSize + NAME_DISPLAY + 20;
		Notice_Col = (COL_SIZE + 2) * ColSize + 1;

		changeWindows(WindowsRow, WindowsCol);

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

	void printWinnerMessage()
	{
		moveCursor(RowSize * ROW_SIZE + 1, 0);
		cout << "The winner is ";
		setColor(BLACK, PlayerColor[WinnerID]);
		for (int i = 0; i < strlen(Player[TurnCounter % 2]); ++i)
		{
			if (Player[TurnCounter % 2][i] != ' ')
				cout << Player[TurnCounter % 2][i];
		}
		setColor(BLACK, WHITE);
		int counter = 0;
		for (int i = 1; i <= RowSize; ++i)
		{
			for (int j = 1; j <= ColSize; ++j)
			{
				counter += (Grid[i][j].CurrentPlayer == WinnerID);
			}
		}
		cout << " after ";
		setColor(BLACK, PlayerColor[WinnerID]);
		cout << counter;
		setColor(BLACK, WHITE);
		cout << " moves";
	}

	int getAndUpdateVertical(int x, int y, int direct = 1, int CurrentPlayer = -1, bool AllowEdit = true)
	{
		int res = 0;
		if (CurrentPlayer == -1)
			CurrentPlayer = Grid[x][y].CurrentPlayer;
		if (CurrentPlayer == -1)
			return 0;
		for (int i = 1; i < WiningCounter; ++i)
		{
			if (Grid[x + i * direct][y].CurrentPlayer != CurrentPlayer)
				break;
			if (AllowEdit)
			{
				if (direct > 0)
					Grid[x + i * direct][y].Top = res + 1;
				else
					Grid[x + i * direct][y].Bottom = res + 1;
			}
			res++;
		}
		return res;
	}
	int getAndUpdateHorizontal(int x, int y, int direct = 1, int CurrentPlayer = -1, bool AllowEdit = true)
	{
		int res = 0;
		if (CurrentPlayer == -1)
			CurrentPlayer = Grid[x][y].CurrentPlayer;
		if (CurrentPlayer == -1)
			return 0;
		for (int i = 1; i < WiningCounter; ++i)
		{
			if (Grid[x][y + i * direct].CurrentPlayer != CurrentPlayer)
				break;
			if (AllowEdit)
			{
				if (direct > 0)
					Grid[x][y + i * direct].Right = res + 1;
				else
					Grid[x][y + i * direct].Left = res + 1;
			}
			res++;
		}
		return res;
	}
	int getAndUpdateMainDiagonal(int x, int y, int direct = 1, int CurrentPlayer = -1, bool AllowEdit = true)
	{
		int res = 0;
		if (CurrentPlayer == -1)
			CurrentPlayer = Grid[x][y].CurrentPlayer;
		if (CurrentPlayer == -1)
			return 0;
		for (int i = 1; i < WiningCounter; ++i)
		{
			if (Grid[x + i * direct][y + i * direct].CurrentPlayer != CurrentPlayer)
				break;
			if (AllowEdit)
			{
				if (direct > 0)
					Grid[x + i * direct][y + i * direct].TopLeft = res + 1;
				else
					Grid[x + i * direct][y + i * direct].BottomRight = res + 1;
			}
			res++;
		}
		return res;
	}
	int getAndUpdateOppositeDiagonal(int x, int y, int direct = 1, int CurrentPlayer = -1, bool AllowEdit = true)
	{
		int res = 0;
		if (CurrentPlayer == -1)
			CurrentPlayer = Grid[x][y].CurrentPlayer;
		if (CurrentPlayer == -1)
			return 0;
		for (int i = 1; i < WiningCounter; ++i)
		{
			if (Grid[x + i * direct][y - i * direct].CurrentPlayer != CurrentPlayer)
				break;
			if (AllowEdit)
			{
				if (direct > 0)
					Grid[x + i * direct][y - i * direct].TopRight = res + 1;
				else
					Grid[x + i * direct][y - i * direct].BottomLeft = res + 1;
			}
			res++;
		}
		return res;
	}

	bool UpdateState(int row, int col)
	{
		Grid[row][col].Top = getAndUpdateVertical(row, col, -1);
		Grid[row][col].Bottom = getAndUpdateVertical(row, col, 1);
		Grid[row][col].Left = getAndUpdateHorizontal(row, col, -1);
		Grid[row][col].Right = getAndUpdateHorizontal(row, col, 1);
		Grid[row][col].TopLeft = getAndUpdateMainDiagonal(row, col, -1);
		Grid[row][col].BottomRight = getAndUpdateMainDiagonal(row, col, 1);
		Grid[row][col].TopRight = getAndUpdateOppositeDiagonal(row, col, -1);
		Grid[row][col].BottomLeft = getAndUpdateOppositeDiagonal(row, col, 1);
		if (Grid[row][col].Top + Grid[row][col].Bottom + 1 >= WiningCounter)
			return true;
		if (Grid[row][col].Left + Grid[row][col].Right + 1 >= WiningCounter)
			return true;
		if (Grid[row][col].TopLeft + Grid[row][col].BottomRight + 1 >= WiningCounter)
			return true;
		if (Grid[row][col].TopRight + Grid[row][col].BottomLeft + 1 >= WiningCounter)
			return true;
		return false;
	}

	bool selectPoint(int row, int col)
	{
		int playerid = TurnCounter % 2;
		if (Grid[row][col].CurrentPlayer != -1)
			return false;
		if (Suggestion.Row != -1)
		{
			setColor(BLACK, WHITE);
			Grid[Suggestion.Row][Suggestion.Col].drawPoint();
			Suggestion = Coordinate(-1, -1);
			moveTo(0,0);
		}
		moveCursor(Grid[row][col].DisplayRow + 2, Grid[row][col].DisplayCol + 2);
		setColor(BLACK, PlayerColor[playerid]);
		cout << Player[playerid];
		Grid[row][col].CurrentPlayer = playerid;
		setColor(BLACK, WHITE);
		MoveOfAll.push(Coordinate(row, col, TurnCounter));
		if (UpdateState(row, col))
		{
			WinnerID = playerid;
			if (UseBot)
			{
				if (WinnerID == 0)
					Accounts_[ActiveID].Win++;
				else
					Accounts_[ActiveID].Lose++;
				saveAccounts();
			}
			printWinnerMessage();
			return false;
		}
		else
		{
			TurnCounter++;
			printNotice();
			if ((TurnRestriction - TurnCounter <= 0 && TurnRestriction != 0) || TurnCounter == RowSize * ColSize)
			{
				moveCursor(RowSize * ROW_SIZE + 1, 0);
				cout << "The game is tie!";
				if (UseBot)
				{
					Accounts_[ActiveID].Tie++;
					saveAccounts();
				}
				WinnerID = 3;
				return false;
			};
		}
		return true;
	}

	Coordinate getSuggestion(int id)
	{
		int take = -1;
		Coordinate prevent = Coordinate(0, 0), attack = Coordinate(0, 0);
		int chance_prevent = 0, chance_attack = 0;
		int top, left, right, bottom, topleft, topright, bottomleft, bottomright;

		//find attack
		for (int i = 1; i <= RowSize; ++i)
		{
			for (int j = 1; j <= ColSize; ++j)
			{
				if (Grid[i][j].CurrentPlayer != -1)
					continue;
				top = getAndUpdateVertical(i, j, -1, id, false);
				bottom = getAndUpdateVertical(i, j, 1, id, false);
				left = getAndUpdateHorizontal(i, j, -1, id, false);
				right = getAndUpdateHorizontal(i, j, 1, id, false);
				topleft = getAndUpdateMainDiagonal(i, j, -1, id, false);
				bottomright = getAndUpdateMainDiagonal(i, j, 1, id, false);
				topright = getAndUpdateOppositeDiagonal(i, j, -1, id, false);
				bottomleft = getAndUpdateOppositeDiagonal(i, j, 1, id, false);
				take = getRandomNumber(0, 100) % 2;
				if (top + bottom + 1 > chance_attack)
				{
					chance_attack = top + bottom + 1;
					attack = Coordinate(i, j);
				}
				else if (top + bottom + 1 == chance_attack && take)
				{
					chance_attack = top + bottom + 1;
					attack = Coordinate(i, j);
				}
				if (left + right + 1 > chance_attack)
				{
					chance_attack = left + right + 1;
					attack = Coordinate(i, j);
				}
				else if (left + right + 1 == chance_attack && take)
				{
					chance_attack = left + right + 1;
					attack = Coordinate(i, j);
				}
				if (topleft + bottomright + 1 > chance_attack)
				{
					chance_attack = topleft + bottomright + 1;
					attack = Coordinate(i, j);
				}
				else if (topleft + bottomright + 1 == chance_attack && take)
				{
					chance_attack = topleft + bottomright + 1;
					attack = Coordinate(i, j);
				}
				if (topright + bottomleft + 1 > chance_attack)
				{
					chance_attack = topright + bottomleft + 1;
					attack = Coordinate(i, j);
				}
				else if (topright + bottomleft + 1 == chance_attack && take)
				{
					chance_attack = topright + bottomleft + 1;
					attack = Coordinate(i, j);
				}
			}
		}

		//find prevent
		for (int i = 1; i <= RowSize; ++i)
		{
			for (int j = 1; j <= ColSize; ++j)
			{
				if (Grid[i][j].CurrentPlayer != -1)
					continue;
				top = getAndUpdateVertical(i, j, -1, (id + 1) % 2, false);
				bottom = getAndUpdateVertical(i, j, 1, (id + 1) % 2, false);
				left = getAndUpdateHorizontal(i, j, -1, (id + 1) % 2, false);
				right = getAndUpdateHorizontal(i, j, 1, (id + 1) % 2, false);
				topleft = getAndUpdateMainDiagonal(i, j, -1, (id + 1) % 2, false);
				bottomright = getAndUpdateMainDiagonal(i, j, 1, (id + 1) % 2, false);
				topright = getAndUpdateOppositeDiagonal(i, j, -1, (id + 1) % 2, false);
				bottomleft = getAndUpdateOppositeDiagonal(i, j, 1, (id + 1) % 2, false);
				take = getRandomNumber(0, 100) % 2;
				if (top + bottom + 1 > chance_prevent)
				{
					chance_prevent = top + bottom + 1;
					prevent = Coordinate(i, j);
				}
				else if (top + bottom + 1 == chance_prevent && take)
				{
					chance_prevent = top + bottom + 1;
					prevent = Coordinate(i, j);
				}
				if (left + right + 1 > chance_prevent)
				{
					chance_prevent = left + right + 1;
					prevent = Coordinate(i, j);
				}
				else if (left + right + 1 == chance_prevent && take)
				{
					chance_prevent = left + right + 1;
					prevent = Coordinate(i, j);
				}
				if (topleft + bottomright + 1 > chance_prevent)
				{
					chance_prevent = topleft + bottomright + 1;
					prevent = Coordinate(i, j);
				}
				else if (topleft + bottomright + 1 == chance_prevent && take)
				{
					chance_prevent = topleft + bottomright + 1;
					prevent = Coordinate(i, j);
				}
				if (topright + bottomleft + 1 > chance_prevent)
				{
					chance_prevent = topright + bottomleft + 1;
					prevent = Coordinate(i, j);
				}
				else if (topright + bottomleft + 1 == chance_prevent && take)
				{
					chance_prevent = topright + bottomleft + 1;
					prevent = Coordinate(i, j);
				}
			}
		}

		if (chance_attack >= WiningCounter)
			return attack;
		if (chance_prevent >= WiningCounter - 1)
			return prevent;
		if (chance_attack > chance_prevent)
			return attack;
		else
			return prevent;
	}

	int navigateToPoint()
	{
		while (true)
		{
			char c = _getch();
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
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
				case 'M':
				case 'm':
					if (WinnerID != -1)
						continue;
					if (Suggestion.Row != -1) continue;
					Suggestion = getSuggestion(TurnCounter % 2);
					setColor(BLACK, PlayerColor[TurnCounter % 2]);
					Grid[Suggestion.Row][Suggestion.Col].drawPoint();
					setColor(BLACK, WHITE);
					moveTo(0,0);
					break;
				case 'R':
				case 'r':
					return 1;
				case 'Q':
				case 'q':
					return 2;
				case 'E':
				case 'e':
					return 0;
				case 'U':
				case 'u':
					if (MoveOfAll.empty())
						break;
					if (Suggestion.Row != -1)
					{
						setColor(BLACK, WHITE);
						Grid[Suggestion.Row][Suggestion.Col].drawPoint();
						Suggestion = Coordinate(-1, -1);
						moveTo(0,0);
					}
					Coordinate point = MoveOfAll.pop();
					Grid[point.Row][point.Col].Initialize();
					Grid[point.Row][point.Col].drawPoint();
					TurnCounter = point.Turn;
					if (UseBot)
					{
						Coordinate point = MoveOfAll.pop();
						Grid[point.Row][point.Col].Initialize();
						Grid[point.Row][point.Col].drawPoint();
						TurnCounter = point.Turn;
					}
					moveTo(0, 0);
					printNotice();
					break;
				}
			}
			else if (c == '\n' || c == '\r' || c == ' ')
			{
				if (WinnerID != -1)
					continue;
				bool canSelect = selectPoint(CurrentRow, CurrentCol);
				if (canSelect && UseBot && WinnerID == -1)
				{
					Coordinate destination = getSuggestion(1);
					selectPoint(destination.Row, destination.Col);
				}
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
		return 0;
	}
};

int printName()
{
	//change windows size
	changeWindows(40, 90);
	int i = 5;
	const int col = 7;
	moveCursor(i, col);
	ifstream file;
	file.open("Name.txt");
	char c;
	setColor(BLACK, 11);
	while (!file.eof())
	{
		c = file.get();
		if (c == '\n' || c == '\r')
		{
			i++;
			moveCursor(i, col);
		}
		else
			cout << c;
	}
	setColor(BLACK, WHITE);
	return i;
}

bool printLogin()
{
	system("cls");
	showCursor(false);
	int LastRow = printName() + 6;
	const int col = 28;
	int pos = 0;
	int pre = 0;
	char Username[NAME_DISPLAY + 1], Password[MAX_PAS + 1];
	setColor(BLACK, 14);
	moveCursor(LastRow, col);
	cout << "Username:  ";
	moveCursor(LastRow + 2, col);
	cout << "Password:  ";
	moveCursor(LastRow + 4, col + 10);
	setColor(GRAY, BLACK);
	cout << " Register ";
	moveCursor(LastRow + 6, col + 10);
	cout << " Login    ";
	moveCursor(LastRow + 8, col + 10);
	cout << " Remove   ";
	moveCursor(LastRow + 10, col + 10);
	cout << " Back     ";
	setColor(BLACK, WHITE);
	moveCursor(LastRow, col + 10);
	cout << '>';
	cout << '\b';
	Username[0] = '\0';
	Password[0] = '\0';
	while (true)
	{
		char c = _getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			pre = pos;
			switch (c)
			{
			case 'W':
			case 'w':
				if (pos == 0)
					pos = 6;
				pos = abs((pos - 1) % 6);
				break;
			case 'S':
			case 's':
				pos = abs((pos + 1) % 6);
				break;
			}
		}
		else if (c == '\n' || c == '\r' || c == ' ')
		{
			moveCursor(LastRow + 12, col);
			cout << "                                            ";
			switch (pos)
			{
			case 0:
				moveCursor(LastRow, col + 12);
				inputCharArray(Username, NAME_DISPLAY + 1);
				break;
			case 1:
				moveCursor(LastRow + 2, col + 12);
				inputCharArray(Password, MAX_PAS + 1, '*');
				break;
			case 2:
				switch (registerAccount(Username, Password))
				{
				case -2:
					moveCursor(LastRow + 12, col + 2);
					setColor(RED, WHITE);
					cout << " Username cannot empty! ";
					setColor(BLACK, WHITE);
					break;
				case 1:
					return false;
				default:
					moveCursor(LastRow + 12, col + 6);
					setColor(RED, WHITE);
					cout << " Username exists! ";
					setColor(BLACK, WHITE);
					break;
				}
				break;
			case 3:
			case 4:
				switch (loginAccount(Username, Password))
				{
				case -2:
					moveCursor(LastRow + 12, col + 2);
					setColor(RED, WHITE);
					cout << " Username cannot empty! ";
					setColor(BLACK, WHITE);
					break;
				case -1:
					moveCursor(LastRow + 12, col + 2);
					setColor(RED, WHITE);
					cout << " Username does not exists! ";
					setColor(BLACK, WHITE);
					break;
				case 0:
					if (pos == 4)
					{
						removeAccount(Username);
						moveCursor(LastRow + 12, col + 10);
						setColor(YELLOW, BLACK);
						cout << " Removed! ";
						setColor(BLACK, WHITE);
						break;
					}
					if (pos == 3)
						return false;
				case 1:
					moveCursor(LastRow + 12, col + 7);
					setColor(RED, WHITE);
					cout << " Wrong password! ";
					setColor(BLACK, WHITE);
					break;
				}
				break;
			case 5:
				return true;
			}
			if (pos < 2)
			{
				pre = pos;
				pos = abs((pos + 1) % 6);
			}
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				pre = pos;
				if (pos == 0)
					pos = 6;
				pos = abs((pos - 1) % 6);
				break;
			case KEY_DOWN:
				pre = pos;
				pos = abs((pos + 1) % 6);
				break;
			}
		}
		if (pre < 2)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pre * 2, col + 10);
			cout << ' ';
		}
		else
		{
			moveCursor(LastRow + pre * 2, col + 10);
			setColor(GRAY, BLACK);
			switch (pre)
			{
			case 2:
				cout << " Register ";
				break;
			case 3:
				cout << " Login    ";
				break;
			case 4:
				cout << " Remove   ";
				break;
			case 5:
				cout << " Back     ";
				break;
			}
		}
		if (pos < 2)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pos * 2, col + 10);
			cout << '>';
		}
		else
		{
			moveCursor(LastRow + pos * 2, col + 10);
			setColor(GREEN, BLACK);
			switch (pos)
			{
			case 2:
				cout << " Register ";
				break;
			case 3:
				cout << " Login    ";
				break;
			case 4:
				cout << " Remove   ";
				break;
			case 5:
				cout << " Back     ";
				break;
			}
		}
		setColor(BLACK, WHITE);
	}
}

bool printMultiSetting(Map &Board)
{
	system("cls");
	showCursor(false);
	int LastRow = printName() + 4;
	const int col = 12;
	int pos = 0;
	int pre = 0;
	const int CURSOR_COL = strlen("Time restriction (less or equal than 100, 0 is off):");
	moveCursor(LastRow, 1);
	cout << "COLOR CODE\n ";
	for (int i = 1; i <= MAX_COLOR_CODE; ++i)
	{
		setColor(BLACK, i);
		cout << i << ' ';
		if (i % 3 == 0)
			cout << "\n ";
	}
	setColor(BLACK, 14);
	moveCursor(LastRow, col);
	cout << "Rows (3 to " << MAX_ROW - 2 << "):";
	moveCursor(LastRow, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << '3';
	setColor(BLACK, 14);
	moveCursor(LastRow + 2, col);
	cout << "Columns (3 to " << MAX_COL - 2 << "):";
	moveCursor(LastRow + 2, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << '3';
	setColor(BLACK, 14);
	moveCursor(LastRow + 4, col);
	cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
	moveCursor(LastRow + 4, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "3";
	setColor(BLACK, 14);
	moveCursor(LastRow + 4, col + CURSOR_COL + 6);
	cout << "symbols";
	moveCursor(LastRow + 6, col);
	cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
	moveCursor(LastRow + 6, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "0";
	setColor(BLACK, 14);
	moveCursor(LastRow + 6, col + CURSOR_COL + 6);
	cout << "turns";
	moveCursor(LastRow + 8, col);
	cout << "First player name:";
	setColor(BLACK, WHITE);
	moveCursor(LastRow + 8, col + CURSOR_COL + 2);
	cout << Board.Player[0];
	setColor(BLACK, 14);
	moveCursor(LastRow + 10, col);
	cout << "First player color:";
	moveCursor(LastRow + 10, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "4";
	setColor(BLACK, 14);
	moveCursor(LastRow + 12, col);
	cout << "Second player name:";
	setColor(BLACK, WHITE);
	moveCursor(LastRow + 12, col + CURSOR_COL + 2);
	cout << Board.Player[1];
	setColor(BLACK, 14);
	moveCursor(LastRow + 14, col);
	cout << "Second player color:";
	moveCursor(LastRow + 14, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "10";
	setColor(BLACK, 14);
	moveCursor(LastRow + 16, col);
	cout << "Background Sound:";
	setColor(BLACK, WHITE);
	moveCursor(LastRow + 16, col + CURSOR_COL + 2);
	cout << "Off";

	setColor(GRAY, BLACK);
	moveCursor(LastRow + 18, col + 30);
	cout << " Next ";
	moveCursor(LastRow + 20, col + 30);
	cout << " Back ";
	setColor(BLACK, WHITE);
	moveCursor(LastRow, col + CURSOR_COL);
	cout << '>';
	cout << '\b';
	while (true)
	{
		char c = _getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			pre = pos;
			switch (c)
			{
			case 'W':
			case 'w':
				if (pos == 0)
					pos = 11;
				pos = abs((pos - 1) % 11);
				break;
			case 'S':
			case 's':
				pos = abs((pos + 1) % 11);
				break;
			}
		}
		else if (c == '\n' || c == '\r' || c == ' ')
		{
			setColor(BLACK, WHITE);
			switch (pos)
			{
			case 0:
				showCursor(true);
				moveCursor(LastRow, col + CURSOR_COL + 2);
				Board.RowSize = inputPositiveInteger(3, MAX_ROW - 2);
				showCursor(false);
				moveCursor(LastRow, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow, col + CURSOR_COL + 2);
				cout << Board.RowSize;
				setColor(BLACK, 14);
				moveCursor(LastRow + 4, col);
				cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
				moveCursor(LastRow + 6, col);
				cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
				setColor(BLACK, WHITE);
				break;
			case 1:
				showCursor(true);
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				Board.ColSize = inputPositiveInteger(3, MAX_COL - 2);
				showCursor(false);
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				cout << Board.ColSize;
				setColor(BLACK, 14);
				moveCursor(LastRow + 4, col);
				cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
				moveCursor(LastRow + 6, col);
				cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
				setColor(BLACK, WHITE);
				break;
			case 2:
				showCursor(true);
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				Board.WiningCounter = inputPositiveInteger(3, min(Board.ColSize, Board.RowSize));
				showCursor(false);
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				cout << Board.WiningCounter;
				break;
			case 3:
				showCursor(true);
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				Board.TurnRestriction = inputPositiveInteger(0, Board.ColSize * Board.RowSize);
				showCursor(false);
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				cout << "   ";
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				cout << Board.TurnRestriction;
				break;
			case 4:
				showCursor(true);
				moveCursor(LastRow + 8, col + CURSOR_COL + 2);
				inputCharArray(Board.Player[0], NAME_DISPLAY + 1);
				showCursor(false);
				break;
			case 5:
				showCursor(true);
				moveCursor(LastRow + 10, col + CURSOR_COL + 2);
				Board.PlayerColor[0] = inputPositiveInteger(1, MAX_COLOR_CODE);
				showCursor(false);
				moveCursor(LastRow + 10, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 10, col + CURSOR_COL + 2);
				cout << Board.PlayerColor[0];
				break;
			case 6:
				showCursor(true);
				moveCursor(LastRow + 12, col + CURSOR_COL + 2);
				inputCharArray(Board.Player[1], NAME_DISPLAY + 1);
				showCursor(false);
				break;
			case 7:
				showCursor(true);
				moveCursor(LastRow + 14, col + CURSOR_COL + 2);
				Board.PlayerColor[1] = inputPositiveInteger(1, MAX_COLOR_CODE);
				showCursor(false);
				moveCursor(LastRow + 14, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 14, col + CURSOR_COL + 2);
				cout << Board.PlayerColor[1];
				break;
			case 8:
				moveCursor(LastRow + 16, col + CURSOR_COL + 2);
				Board.BackgroundSound = !Board.BackgroundSound;
				if (Board.BackgroundSound)
					cout << "On ";
				else
					cout << "Off";
				break;
			case 9:
				Board.rewriteName(0);
				Board.rewriteName(1);
				return false;
			case 10:
				return true;
			}
			if (pos < 9)
			{
				pre = pos;
				pos = abs((pos + 1) % 11);
			}
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				pre = pos;
				if (pos == 0)
					pos = 11;
				pos = abs((pos - 1) % 11);
				break;
			case KEY_DOWN:
				pre = pos;
				pos = abs((pos + 1) % 11);
				break;
			}
		}
		if (pre < 9)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pre * 2, col + CURSOR_COL);
			cout << ' ';
		}
		else
		{
			moveCursor(LastRow + pre * 2, col + 30);
			setColor(GRAY, BLACK);
			switch (pre)
			{
			case 9:
				cout << " Next ";
				break;
			case 10:
				cout << " Back ";
				break;
			}
		}
		if (pos < 9)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pos * 2, col + CURSOR_COL);
			cout << '>';
		}
		else
		{
			moveCursor(LastRow + pos * 2, col + 30);
			setColor(GREEN, BLACK);
			switch (pos)
			{
			case 9:
				cout << " Next ";
				break;
			case 10:
				cout << " Back ";
				break;
			}
		}
		setColor(BLACK, WHITE);
	}
}

bool printSingleSetting(Map &Board)
{
	system("cls");
	showCursor(false);
	int LastRow = printName() + 4;
	const int col = 12;
	int pos = 0;
	int pre = 0;
	const int CURSOR_COL = strlen("Time restriction (less or equal than 100, 0 is off):");
	moveCursor(LastRow, 1);
	cout << "COLOR CODE\n ";
	for (int i = 1; i <= MAX_COLOR_CODE; ++i)
	{
		setColor(BLACK, i);
		cout << i << ' ';
		if (i % 3 == 0)
			cout << "\n ";
	}
	setColor(BLACK, 14);
	moveCursor(LastRow, col);
	cout << "Rows (3 to " << MAX_ROW - 2 << "):";
	moveCursor(LastRow, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << '3';
	setColor(BLACK, 14);
	moveCursor(LastRow + 2, col);
	cout << "Columns (3 to " << MAX_COL - 2 << "):";
	moveCursor(LastRow + 2, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << '3';
	setColor(BLACK, 14);
	moveCursor(LastRow + 4, col);
	cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
	moveCursor(LastRow + 4, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "3";
	setColor(BLACK, 14);
	moveCursor(LastRow + 4, col + CURSOR_COL + 6);
	cout << "symbols";
	moveCursor(LastRow + 6, col);
	cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
	moveCursor(LastRow + 6, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "0";
	setColor(BLACK, 14);
	moveCursor(LastRow + 6, col + CURSOR_COL + 6);
	cout << "turns";
	setColor(BLACK, 14);
	moveCursor(LastRow + 8, col);
	cout << "Player color:";
	moveCursor(LastRow + 8, col + CURSOR_COL + 2);
	setColor(BLACK, WHITE);
	cout << "4";
	setColor(BLACK, 14);
	moveCursor(LastRow + 10, col);
	cout << "Background Sound:";
	setColor(BLACK, WHITE);
	moveCursor(LastRow + 10, col + CURSOR_COL + 2);
	cout << "Off";

	setColor(GRAY, BLACK);
	moveCursor(LastRow + 12, col + 30);
	cout << " Next ";
	moveCursor(LastRow + 14, col + 30);
	cout << " Back ";
	setColor(BLACK, WHITE);
	moveCursor(LastRow, col + CURSOR_COL);
	cout << '>';
	cout << '\b';
	while (true)
	{
		char c = _getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			pre = pos;
			switch (c)
			{
			case 'W':
			case 'w':
				if (pos == 0)
					pos = 8;
				pos = abs((pos - 1) % 8);
				break;
			case 'S':
			case 's':
				pos = abs((pos + 1) % 8);
				break;
			}
		}
		else if (c == '\n' || c == '\r' || c == ' ')
		{
			setColor(BLACK, WHITE);
			switch (pos)
			{
			case 0:
				showCursor(true);
				moveCursor(LastRow, col + CURSOR_COL + 2);
				Board.RowSize = inputPositiveInteger(3, MAX_ROW - 2);
				showCursor(false);
				moveCursor(LastRow, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow, col + CURSOR_COL + 2);
				cout << Board.RowSize;
				setColor(BLACK, 14);
				moveCursor(LastRow + 4, col);
				cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
				moveCursor(LastRow + 6, col);
				cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
				setColor(BLACK, WHITE);
				break;
			case 1:
				showCursor(true);
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				Board.ColSize = inputPositiveInteger(3, MAX_COL - 2);
				showCursor(false);
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 2, col + CURSOR_COL + 2);
				cout << Board.ColSize;
				setColor(BLACK, 14);
				moveCursor(LastRow + 4, col);
				cout << "Win with (3 to " << min(Board.ColSize, Board.RowSize) << "):";
				moveCursor(LastRow + 6, col);
				cout << "Time restriction (less or equal than " << Board.ColSize * Board.RowSize << ", 0 is off):";
				setColor(BLACK, WHITE);
				break;
			case 2:
				showCursor(true);
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				Board.WiningCounter = inputPositiveInteger(3, min(Board.ColSize, Board.RowSize));
				showCursor(false);
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 4, col + CURSOR_COL + 2);
				cout << Board.WiningCounter;
				break;
			case 3:
				showCursor(true);
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				Board.TurnRestriction = inputPositiveInteger(0, Board.ColSize * Board.RowSize);
				showCursor(false);
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				cout << "   ";
				moveCursor(LastRow + 6, col + CURSOR_COL + 2);
				cout << Board.TurnRestriction;
				break;
			case 4:
				showCursor(true);
				moveCursor(LastRow + 8, col + CURSOR_COL + 2);
				Board.PlayerColor[0] = inputPositiveInteger(1, MAX_COLOR_CODE);
				showCursor(false);
				moveCursor(LastRow + 8, col + CURSOR_COL + 2);
				cout << "  ";
				moveCursor(LastRow + 8, col + CURSOR_COL + 2);
				cout << Board.PlayerColor[0];
				break;
			case 5:
				moveCursor(LastRow + 10, col + CURSOR_COL + 2);
				Board.BackgroundSound = !Board.BackgroundSound;
				if (Board.BackgroundSound)
					cout << "On ";
				else
					cout << "Off";
				break;
			case 6:
				strcpy_s(Board.Player[0], Accounts_[ActiveID].Username);
				Board.rewriteName(0);
				strcpy_s(Board.Player[1], "BOT");
				Board.rewriteName(1);
				Board.PlayerColor[1] = ((Board.PlayerColor[0] + 1) % MAX_COLOR_CODE) + 1;
				return false;
			case 7:
				return true;
			}
			if (pos < 6)
			{
				pre = pos;
				pos = abs((pos + 1) % 8);
			}
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				pre = pos;
				if (pos == 0)
					pos = 8;
				pos = abs((pos - 1) % 8);
				break;
			case KEY_DOWN:
				pre = pos;
				pos = abs((pos + 1) % 8);
				break;
			}
		}
		if (pre < 6)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pre * 2, col + CURSOR_COL);
			cout << ' ';
		}
		else
		{
			moveCursor(LastRow + pre * 2, col + 30);
			setColor(GRAY, BLACK);
			switch (pre)
			{
			case 6:
				cout << " Next ";
				break;
			case 7:
				cout << " Back ";
				break;
			}
		}
		if (pos < 6)
		{
			setColor(BLACK, WHITE);
			moveCursor(LastRow + pos * 2, col + CURSOR_COL);
			cout << '>';
		}
		else
		{
			moveCursor(LastRow + pos * 2, col + 30);
			setColor(GREEN, BLACK);
			switch (pos)
			{
			case 6:
				cout << " Next ";
				break;
			case 7:
				cout << " Back ";
				break;
			}
		}
		setColor(BLACK, WHITE);
	}
}

int printGameModeSelection()
{
	system("cls");
	showCursor(false);
	int LastRow = printName() + 6;
	const int col = 36;
	int pos = 0;
	int pre = 0;
	moveCursor(LastRow, col - 19);
	cout << "Please only use English Typing for the best experience";
	LastRow++;
	moveCursor(LastRow, col - 11);
	cout << "Use W,A,S,D or ARROW KEY to move around";
	LastRow++;
	moveCursor(LastRow, col - 7);
	cout << "Use ENTER to select or confirm";
	LastRow += 2;

	moveCursor(LastRow, col);
	setColor(GREEN, BLACK);
	cout << " Singleplayer Mode ";
	setColor(GRAY, BLACK);
	moveCursor(LastRow + 2, col);
	cout << " Multiplayer Mode  ";
	moveCursor(LastRow + 4, col);
	cout << " Exit              ";
	while (true)
	{
		char c = _getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			pre = pos;
			switch (c)
			{
			case 'W':
			case 'w':
				if (pos == 0)
					pos = 3;
				pos = abs((pos - 1) % 3);
				break;
			case 'S':
			case 's':
				pos = abs((pos + 1) % 3);
				break;
			}
		}
		else if (c == '\n' || c == '\r' || c == ' ')
		{
			setColor(BLACK, WHITE);
			return pos;
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				pre = pos;
				if (pos == 0)
					pos = 3;
				pos = abs((pos - 1) % 3);
				break;
			case KEY_DOWN:
				pre = pos;
				pos = abs((pos + 1) % 3);
				break;
			}
		}
		moveCursor(LastRow + pre * 2, col);
		setColor(GRAY, BLACK);
		switch (pre)
		{
		case 0:
			cout << " Singleplayer Mode ";
			break;
		case 1:
			cout << " Multiplayer Mode  ";
			break;
		case 2:
			cout << " Exit              ";
			break;
		}
		moveCursor(LastRow + pos * 2, col);
		setColor(GREEN, BLACK);
		switch (pos)
		{
		case 0:
			cout << " Singleplayer Mode ";
			break;
		case 1:
			cout << " Multiplayer Mode  ";
			break;
		case 2:
			cout << " Exit              ";
			break;
		}
		setColor(BLACK, WHITE);
	}
}

bool playMultiplayerGame() // return true if player want to play again
{
	system("cls");
	Map FullMap = Map();
	if (printMultiSetting(FullMap))
		return true;
	if (FullMap.BackgroundSound) PlaySound(TEXT("background.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

StartMultiGame:
	FullMap.Initialize();
	FullMap.printMap();
	FullMap.UseBot = false;
	int result = FullMap.navigateToPoint();
	switch (result)
	{
	case 1:
		goto StartMultiGame;
	case 2:
		return true;
	default:
		return false;
	}
}

int playSingleGame() // return true if player want to play again
{
	system("cls");
	Map FullMap = Map();
	if (printSingleSetting(FullMap))
		return -1;
	if (FullMap.BackgroundSound) PlaySound(TEXT("background.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

StartSingleGame:
	FullMap.Initialize();
	FullMap.printMap();
	FullMap.UseBot = true;
	int result = FullMap.navigateToPoint();
	switch (result)
	{
	case 1:
		goto StartSingleGame;
	case 2:
		return 1;
	default:
		return 0;
	}
}

bool printSinglePlayerMenu()
{
	system("cls");
	showCursor(false);
	int LastRow = printName() + 6;
	const int col = 28;
	int pos = 0;
	int pre = 0;
	moveCursor(LastRow, col + 10);
	setColor(GREEN, BLACK);
	cout << " Play              ";
	setColor(GRAY, BLACK);
	moveCursor(LastRow + 2, col + 10);
	cout << " Logout            ";
	moveCursor(LastRow + 4, col + 10);
	cout << " Back to Main Menu ";
	setColor(BLACK, YELLOW);
	moveCursor(LastRow + 6, col + 10);
	cout << " Won:  " << Accounts_[ActiveID].Win << "  ";
	moveCursor(LastRow + 7, col + 10);
	cout << " Tie:  " << Accounts_[ActiveID].Tie << "  ";
	moveCursor(LastRow + 8, col + 10);
	cout << " Lose: " << Accounts_[ActiveID].Lose << " ";
	setColor(BLACK, WHITE);
	while (true)
	{
		char c = _getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			pre = pos;
			switch (c)
			{
			case 'W':
			case 'w':
				if (pos == 0)
					pos = 3;
				pos = abs((pos - 1) % 3);
				break;
			case 'S':
			case 's':
				pos = abs((pos + 1) % 3);
				break;
			}
		}
		else if (c == '\n' || c == '\r' || c == ' ')
		{
			switch (pos)
			{
			case 0:
				return false;
			case 1:
				ActiveID = -1;
				return true;
			case 2:
				return true;
			}
		}
		else
		{
			c = _getch();
			switch (c)
			{
			case KEY_UP:
				pre = pos;
				if (pos == 0)
					pos = 3;
				pos = abs((pos - 1) % 3);
				break;
			case KEY_DOWN:
				pre = pos;
				if (pos == 0)
					pos = 3;
				pos = abs((pos + 1) % 3);
				break;
			}
		}
		moveCursor(LastRow + pre * 2, col + 10);
		setColor(GRAY, BLACK);
		switch (pre)
		{
		case 0:
			cout << " Play              ";
			break;
		case 1:
			cout << " Logout            ";
			break;
		case 2:
			cout << " Back to Main Menu ";
			break;
		}
		moveCursor(LastRow + pos * 2, col + 10);
		setColor(GREEN, BLACK);
		switch (pos)
		{
		case 0:
			cout << " Play              ";
			break;
		case 1:
			cout << " Logout            ";
			break;
		case 2:
			cout << " Back to Main Menu ";
			break;
		}
		setColor(BLACK, WHITE);
	}
}

Point Grid[MAX_ROW][MAX_COL];
int n = 0;

int main()
{
	//Disable selection
	SetConsoleTitle(TEXT("TicTacToe - 21127469"));
	SetConsoleMode(InHamdle, ~ENABLE_QUICK_EDIT_MODE);
	loadAccounts();

Starting:
	PlaySound(NULL, 0, 0);
	int mode = printGameModeSelection();
	switch (mode)
	{
	case 0:
	{
	Login:
		if (ActiveID == -1)
		{
			if (printLogin())
				goto Starting;
		}
	Menu:
		if (printSinglePlayerMenu())
		{
			if (ActiveID == -1)
				goto Login;
			else
				goto Starting;
		}
		int res = playSingleGame();
		if (res == 1)
		{
			goto Starting;
		}
		else if (res == -1)
		{
			goto Menu;
		}
		break;
	}
	case 1:
		if (playMultiplayerGame())
			goto Starting;
		break;
	}
	PlaySound(NULL, 0, 0);
	return 0;
}