#include "fSnakeGame.h"
#include <unistd.h>

#include <stdio.h>
#include <time.h>


time_t t1 = time(NULL);

using namespace std;

CharPosition::CharPosition(int col, int row)
{
	x = col;
	y = row;
}

CharPosition::CharPosition()
{
	x = 0;
	y = 0;
}

fSnakeGame::fSnakeGame()
{
	// variables initialisation:
	partchar = pattern[0]; // character to represent the snake
	edgechar = (char)219; // full rectangle on the key table
	fruitchar = '*'; 
	fruit.x = 0;
	fruit.y = 0;
    badfruitchar = 'N'; 
	badfruit.x = 0;
	badfruit.y = 0;
	score = 0;
	del = 150000 - atoi(level)*10000;
	bool bEatsFruit = 0;
	bool bEatsBadFruit = 1; //這邊要改
	direction = 'l';
	srand(time(NULL));
 	/*
	wallNumber = 2;	
	vertical[0] = 1;
	start1[0] = 10;
	start2[0] = 10;
	length[0] = 15;
	
	vertical[1] = 1;
	start1[1] = 20;
	start2[1] = 20;
	length[1] = 15;
	
	vertical[2] = 1;
	start1[2] = 25;
	start2[2] = 25;
	length[2] = 15;
	*/
	 
	InitGameWindow();

	
	wallNumber = (MaxWallNumber / 2) + (rand() % (MaxWallNumber / 2)) + 1; //牆的數量是隨機的 (6~10) 
	//cout << "wallNumber=" << wallNumber << endl;
	for(int i = 0; i < wallNumber; i ++){
		vertical[i] = rand() % 2; //0 OR 1
		//cout << "vertical=" << vertical[i] << endl;
		if (vertical[i] == 1){ //垂直 
			start1[i] = rand() % (maxwidth - 2) + 1;
			start2[i] = rand() % (maxheight - 3) + 1;
			length[i] = rand() % (maxheight / 2); //讓最大長度不要超過高度一半 
		} else { //水平 
			start1[i] = rand() % (maxheight - 3) + 1;
			start2[i] = rand() % (maxwidth - 2) + 1;
			length[i] = rand() % (maxwidth / 2);
		}
	}
	
	
	/*
	for (int i = 0; i < MaxWallNumber; i ++){
		cout << vertical[i] << "," << start1[i] << "," << start2[i] << "," << length[i] << endl;
	}
	*/
	
	PositionFruit();
    PositionBadFruit();
	DrawWindow();
	DrawWall();
	DrawSnake();
	PrintScore();
	PrintName();

	refresh();	
}

fSnakeGame::~fSnakeGame()
{
	nodelay(stdscr, false);
	getch();
	endwin();
}

// initialise the game window
void fSnakeGame::InitGameWindow() 
{ 
	initscr(); // initialise the screen
	start_color(); // we can now use colours in the terminal
	init_pair(1, COLOR_WHITE, COLOR_YELLOW); 
	nodelay(stdscr,TRUE);
	keypad(stdscr, true); // initialise the keyboard: we can use arrows for directions
	noecho(); // user input is not displayed on the screen
	curs_set(0); // cursor symbol is not not displayed on the screen (Linux)
	getmaxyx(stdscr, maxheight, maxwidth); // define dimensions of game window
	return; 
}

// draw the game window
void fSnakeGame::DrawWindow()
{
	for (int i = 0; i < maxwidth; i++) // draws top
	{
		move(0, i);
		addch(edgechar);
	}

	for (int i = 0; i < maxwidth; i++) // draws bottom
	{
		move(maxheight-2, i);
		addch(edgechar);
	}

	for (int i = 0; i < maxheight-1; i++) // draws left side
	{
		move(i, 0);
		addch(edgechar);
	}

	for (int i = 0; i < maxheight-1; i++) // draws right side
	{
		move(i, maxwidth-1);
		addch(edgechar);
	}
	
	return;
}

void fSnakeGame::DrawWall()
{
	for (int i = 0; i < wallNumber; i ++){
		if (vertical[i]){ //垂直 
			int col = start1[i]; //column 
			for (int j = 0; j < length[i]; j ++){
				int row = start2[i] + j;
				row = row % (maxheight - 2); //多出來的從上面開始畫 
				move(row, col);
				addch(edgechar);
			}
		} else {
			int row = start1[i];
			for (int j = 0; j < length[i]; j ++){
				int col = start2[i] + j;
				col = col % (maxwidth - 1);
				move(row, col);
				addch(edgechar);
			}
		}
	}


	return;
}

// draw snake's body
void fSnakeGame::DrawSnake()
{
	for (int i = 0; i < 5; i++) 
	{
		snake.push_back(CharPosition(30+i, 10));
	}

	for (int i = 0; i < snake.size(); i++)
	{
		move(snake[i].y, snake[i].x);
		addch(partchar);
	}
	return;
}

// print score at bottom of window
void fSnakeGame::PrintScore()
{
	move(maxheight-1, 0);
	//printw("Score: %d", score);
	printw("Score: %d, Fruit @ (%d, %d)", score, fruit.x, fruit.y);
	return;
}

// print UserName at bottom of window
void fSnakeGame::PrintName()
{
	move(maxheight - 1, 15);
	printw("Player: ");
	printw(name);
	return;
}

// position a new fruit in the game window
void fSnakeGame::PositionFruit()
{
	while(1)
	{
		int tmpx = rand()%maxwidth+1; // +1 to avoid the 0
		int tmpy = rand()%maxheight+1;

		// check that the fruit is not positioned on the snake
		for (int i = 0; i < snake.size(); i++)
		{
			if (snake[i].x == tmpx && snake[i].y == tmpy)
			{
				continue; // if true, ignore the following and go back to the beginning of function
			}
		}

		// check that the fruit is positioned within the game window
		if (tmpx >= maxwidth-2 || tmpy >= maxheight-3)
		{
			continue; // if true, ignore the following and go back to the beginning of function
		}
		
		// check that the fruit is positioned within the walls
		bool conflict = false;
		for (int i = 0; i < wallNumber; i ++){
			if (vertical[i]){ //垂直 
				if (tmpx == start1[i]){
					if (tmpy >= start2[i] && tmpy < start2[i] + length[i]){
						conflict = true;
						continue;
					}
					if ((tmpy + maxheight - 3) < start2[i] + length[i]){
						conflict = true;
						continue;
					}
				}
			} else {
				if (tmpy == start1[i]){
					if (tmpx >= start2[i] && tmpx < start2[i] + length[i]){
						conflict = true;
						continue;
					}
					if ((tmpx + maxwidth - 2) < start2[i] + length[i]){
						conflict = true;
						continue;
					}
				}
			}
		}
		if(conflict)
			continue;

		// if the coordinates are valid, add fruit in the window
		fruit.x = tmpx;
		fruit.y = tmpy;
		break;
	}

	move(fruit.y, fruit.x); 
	addch(fruitchar);
	refresh();
}


// 毒蘋果
void fSnakeGame::PositionBadFruit()
{
	while(1)
	{
		int tmpx = rand()%maxwidth+1; // +1 to avoid the 0
		int tmpy = rand()%maxheight+1;

		// check that the fruit is not positioned on the snake
		for (int i = 0; i < snake.size(); i++)
		{
			if (snake[i].x == tmpx && snake[i].y == tmpy)
			{
				continue; // if true, ignore the following and go back to the beginning of function
			}
		}

		// check that the fruit is positioned within the game window
		if (tmpx >= maxwidth-2 || tmpy >= maxheight-3)
		{
			continue; // if true, ignore the following and go back to the beginning of function
		}
		
		// check that the fruit is positioned within the walls
		bool conflict = false;
		for (int i = 0; i < wallNumber; i ++){
			if (vertical[i]){ //垂直 
				if (tmpx == start1[i]){
					if (tmpy >= start2[i] && tmpy < start2[i] + length[i]){
						conflict = true;
						continue;
					}
					if ((tmpy + maxheight - 3) < start2[i] + length[i]){
						conflict = true;
						continue;
					}
				}
			} else {
				if (tmpy == start1[i]){
					if (tmpx >= start2[i] && tmpx < start2[i] + length[i]){
						conflict = true;
						continue;
					}
					if ((tmpx + maxwidth - 2) < start2[i] + length[i]){
						conflict = true;
						continue;
					}
				}
			}
		}
		if(conflict)
			continue;

		// if the coordinates are valid, add fruit in the window
		badfruit.x = tmpx;
		badfruit.y = tmpy;
		break;
	}

	move(badfruit.y, badfruit.x); 
	addch(badfruitchar);
	refresh();
}

// set game over situations
bool fSnakeGame::FatalCollision()
{
	// if the snake hits the edge of the window
	if (snake[0].x == 0 || snake[0].x == maxwidth-1 || snake[0].y == 0 || snake[0].y == maxheight-2)
	{
		return true;
	}

	// if the snake collides into himself
	for (int i = 2; i < snake.size(); i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			return true;
		}
	}

	return false;
}

bool fSnakeGame::WallCollision()
{
	// if the snake hits the edge of the window
	/*
	if (snake[0].x == 5){
		if (snake[0].y >= 5 && snake[0].y < 15) return true;
	}

	if (snake[0].y == 20){
		if (snake[0].x >= 20 && snake[0].x < 36) return true;
	}
	*/
	for (int i = 0; i < wallNumber; i ++){
		if (vertical[i]){ //垂直 
			if (snake[0].x == start1[i]){
				if (snake[0].y >= start2[i] && snake[0].y < start2[i] + length[i]) return true;
				if ((snake[0].y + maxheight - 3) < start2[i] + length[i] - 1) return true; //上面碰撞 
			}
		} else {
			if (snake[0].y == start1[i]){
				if (snake[0].x >= start2[i] && snake[0].x < start2[i] + length[i]) return true;
				if ((snake[0].x + maxwidth - 2) < start2[i] + length[i] - 1) return true;
			}
		}
	}
	
	return false;
}

// define behaviour when snake eats the fruit
bool fSnakeGame::GetsFruit()
{
	if (snake[0].x == fruit.x && snake[0].y == fruit.y)
	{
		PositionFruit(); 
		score +=10; 
		PrintScore();

		// if score is a multiple of 100, increase snake speed
		if ((score%50) == 0)
		{
			del -= 1000;
		}
		return bEatsFruit = true;
	}
	else 
	{
		return bEatsFruit = false;
	}
	return bEatsFruit;
}

bool fSnakeGame::GetsBadFruit()
{	
	
	if (snake[0].x == badfruit.x && snake[0].y == badfruit.y)//無限增生........
	{
		PositionBadFruit(); 
		score -= 10; 
		PrintScore();

		return bEatsBadFruit = false;
	}
	/*
	else if(t1%1 == 0){
		PositionBadFruit(); 
		return bEatsBadFruit = false;//無限增生........
	}
	*/
	else 
	{
		return bEatsBadFruit = true;
	}
	return bEatsBadFruit;
	
}

// define snake's movements
void fSnakeGame::MoveSnake()
{
	int KeyPressed = getch();
	switch(KeyPressed)
	{
		case KEY_LEFT:
			if (direction != 'r') 
			{ direction = 'l'; }  
			break;
		case KEY_RIGHT:
			if (direction != 'l')
			{ direction = 'r'; }
			break;
		case KEY_UP:
			if (direction != 'd')
			{ direction = 'u'; }
			break;
		case KEY_DOWN:
			if (direction != 'u')
			{ direction = 'd'; }
			break;
		case KEY_BACKSPACE:
			direction = 'q'; // key to quit the game
			break;
	}

	// the snake doesn't eat fruit, remains same size
	if (!bEatsFruit)
	{
		move(snake[snake.size()-1].y, snake[snake.size()-1].x); // moves at the end of the tail
		printw(" "); // add empty ch to remove last character
		refresh();
		snake.pop_back(); // removes the last element in the vector, reducing the container size by one
	}

	// the snake moves and we add an extra character at the beginning of the vector
	// add a head and initialise new coordinates for CharPosition according to the direction input
	if (direction == 'l')
	{ snake.insert(snake.begin(), CharPosition(snake[0].x-1, snake[0].y)); } 
	else if (direction == 'r')
	{ snake.insert(snake.begin(), CharPosition(snake[0].x+1, snake[0].y)); }
	else if (direction == 'u')
	{ snake.insert(snake.begin(), CharPosition(snake[0].x, snake[0].y-1)); }
	else if (direction == 'd')
	{ snake.insert(snake.begin(), CharPosition(snake[0].x, snake[0].y+1)); }

	// move to the new CharPosition coordinates
	move(snake[0].y, snake[0].x);
	addch(partchar); // add a new head
	refresh();
	return;
}

void fSnakeGame::PlayGame()
{
    while(1)
    {
        if (FatalCollision() || WallCollision())
        {
            move((maxheight-2)/2,(maxwidth-5)/2);
            printw("GAME OVER");
            break;
        }

        GetsFruit();	
		GetsBadFruit();
        MoveSnake();

        if (direction=='q') //exit
        {
        	break;
        }

        usleep(del); // delay
    }
}
