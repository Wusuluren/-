/*
 * Had you played the game "catch the nervous cat"? if not, now have a try!
 * how to play: up:w, down:s, left:a, right:d, enter:space.
 * this is a shabby version, and next version i will make the cat smarter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define BLANK 'O'
#define STONE '*'
#define CAT '@'

#define LINE 9
#define COLUMN 9
#define NUM_STONE_INIT_MAX 10
#define CON_LEN_MAX 5

enum type {
	blank = 0,
	stone = 2,
	cat = 3,
	visited = 4
};
enum who {
	PLAYER = 1,
	COMPUTER = 2
};
enum direction {
	LEFT = 0,
	LEFT_UP = 1,
	RIGHT_UP = 2,
	RIGHT = 3,
	RIGHT_DOWN = 4,
	LEFT_DOWN = 5,
};

int a[LINE][COLUMN] = {0};
int cat_x, cat_y;
int player_x, player_y;
int dir;
const char *info[] = {"You win!", "You lost!"};
int min_step, next_x, next_y;

void init();
void player();
void computer();
void init_map();
void draw_map();
void who_win(int who);

int main()
{
	init();
	while(1) {
		player();
		computer();
	}

	return EXIT_SUCCESS;
}

void who_win(int who)
{
	if(who == player) {
		move(9, 0);
		printw("%s", info[0]);
		refresh();
	}
	else {
		move(9, 0);
		printw("%s", info[1]);
		refresh();
	}
	sleep(2);

	endwin();
	exit(EXIT_SUCCESS);
}

void computer()
{
	int cnt = 0;

_retry:
	if(cnt > 5)
		who_win(player);
	if((cat_x == 0) || (cat_x == 8) || 
			(cat_y == 0) || (cat_y == 8))
		who_win(computer);

	if(LEFT == dir) {
		if(a[cat_x - 1][cat_y] == blank) {
			next_x = cat_x - 1;
			next_y = cat_y;
		}
		else {
			dir = (dir + 1) % 6;
			cnt++;
			goto _retry;
		}
	}
	else if(RIGHT == dir) {
		if(a[cat_x + 1][cat_y] == blank) {
			next_x = cat_x + 1;
			next_y = cat_y;
		}
		else {
			dir = (dir + 1) % 6;
			cnt++;
			goto _retry;
		}
	}
	else if((cat_x % 2) == 1) {
		if(LEFT_UP == dir) {
			if(a[cat_x - 1][cat_y] == blank) {
				next_x = cat_x - 1;
				next_y = cat_y;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(LEFT_DOWN == dir) { 
			if(a[cat_x + 1][cat_y] == blank) {
				next_x = cat_x + 1;
				next_y = cat_y;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(RIGHT_UP == dir) {
			if(a[cat_x - 1][cat_y + 1] == blank) {
				next_x = cat_x - 1;
				next_y = cat_y + 1;
			} 
			else {
				dir= (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(RIGHT_DOWN == dir) {
			if(a[cat_x + 1][cat_y + 1] == blank) {
				next_x = cat_x + 1;
				next_y = cat_y + 1;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
	}
	else if((cat_x % 2) == 0) {
		if(LEFT_UP == dir) {
			if(a[cat_x - 1][cat_y - 1] == blank) {
				next_x = cat_x - 1;
				next_y = cat_y - 1;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(LEFT_DOWN == dir) { 
			if(a[cat_x + 1][cat_y - 1] == blank) {
				next_x = cat_x + 1;
				next_y = cat_y - 1;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(RIGHT_UP == dir) {
			if(a[cat_x - 1][cat_y] == blank) {
				next_x = cat_x - 1;
				next_y = cat_y;
			}
			else {
				dir= (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
		else if(RIGHT_DOWN == dir) {
			if(a[cat_x + 1][cat_y] == blank) {
				next_x = cat_x + 1;
				next_y = cat_y;
			}
			else {
				dir = (dir + 1) % 6;
				cnt++;
			goto _retry;
			}
		}
	}
	else {
		who_win(player);
	}


	if((next_x % 2) == 0) 
		move(next_x, next_y * 2);
	else
		move(next_x, next_y * 2 + 1);
	addch(CAT);
	refresh();

	if((cat_x % 2) == 0) 
		move(cat_x, cat_y * 2);
	else
		move(cat_x, cat_y * 2 + 1);
	addch(BLANK);
	refresh();

	cat_x = next_x;
	cat_y = next_y;
}

void init()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	srand(time(NULL));
	init_map();
	draw_map();
	dir = rand() % 6;
}

void player()
{
	char ch, oldc;
	int old_x, old_y;

	old_x = player_x;
	old_y = player_y;
	while(ch = getch()) {
		switch(ch) {
			case 'w':
			case 'W':
				if(player_x == 0)
					player_x = 8;
				else
					player_x--;
				break;
			case 's':
			case 'S':
				if(player_x == 8)
					player_x = 0;
				else
					player_x++;
				break;
			case 'a':
			case 'A':
				if(player_y == 0)
					player_y = 8;
				else 
					player_y--;
				break;
			case 'd':
			case 'D':
				if(player_y == 8)
					player_y = 0;
				else
					player_y++;
				break;
			case ' ':
				a[player_x][player_y] = stone;
				if((player_x % 2) == 0)
					move(player_x, player_y * 2);
				else
					move(player_x, player_y * 2 + 1);
				if((char)inch() != CAT) {
					attron(A_STANDOUT);
					addch(STONE);
					refresh();
					attroff(A_STANDOUT);
				}
				break;
			default:
				break;
		}// end switch

		if(ch == ' ')
			return;

		if((old_x % 2) == 0) 
			move(old_x, old_y * 2);
		else
			move(old_x, old_y * 2 + 1);
		oldc = (char)inch();
		addch(oldc);
		refresh();

		if((player_x % 2) == 0)
			move(player_x, player_y * 2);
		else
			move(player_x, player_y * 2 + 1);
		attron(A_STANDOUT);
		oldc = (char)inch();
		addch(oldc);
		attroff(A_STANDOUT);
		refresh();

		old_x = player_x;
		old_y = player_y;
	}// end for
}

void init_map()
{
	int num, i, j, x, y;

	player_x = 5;
	player_y = 5;
	a[player_x][player_y] = stone;
	cat_x = 4;
	cat_y = 4;
	a[cat_x][cat_y] = cat;

	num = NUM_STONE_INIT_MAX;
	for(i = 0; i < num; i++) {
		x = rand() % LINE;
		y = rand() % COLUMN;
		if((x == cat_x) && (y == cat_y)) {
			continue;
		}
		a[x][y] = stone;
	} //end for
}

void draw_map()
{
	int i, j, k, col;

	for(i = 0; i < 9; i++) {
		for(j = 0; j < 18; j++) {
			if((i % 2) == 0) {
				move(i, j);
				switch(a[i][j / 2]) {
					case blank:
						addch(BLANK);
						break;
					case stone:
						addch(STONE);
						break;
					case cat:
						addch(CAT);					
						break;
					default:
						break;
				}
				refresh();
				move(i, ++j);
				addch(' ');
				refresh();
			}
			else if((i % 2) == 1) {
				move(i, j);
				addch(' ');
				refresh();
				move(i, ++j);
				switch(a[i][j / 2]) {
					case blank:
						addch(BLANK);
						break;
					case stone:
						addch(STONE);
						break;
					case cat:
						addch(CAT);					
						break;
					default:
						break;
				}
				refresh();
			}
		}
	}
	move(player_x, player_y * 2 + 1);
	attron(A_STANDOUT);
	addch(STONE);
	refresh();
	attroff(A_STANDOUT);
}
