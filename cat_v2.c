/*
 * compile: gcc cat_v2.c -lcurses
 * how to play: w->up, s->down, a->left, d->right, space->enter 
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <setjmp.h>

#define CWAY 'O'
#define CSTONE '*'
#define CCAT '@'
#define CCAT_IN_CIRCLE '#'

#define ROW 9
#define COL 9
#define INIT_STONE 10
#define HEAP_SIZE ROW*COL+10

enum Type {
	WAY = 1,
	STONE = 2,
	CAT = 3,
};
enum Who {
	PLAYER = 1,
	COMPUTER = 2
};
enum Direction {
	LEFT = 0,
	LEFT_UP = 1,
	RIGHT_UP = 2,
	RIGHT = 3,
	RIGHT_DOWN = 4,
	LEFT_DOWN = 5
};

typedef struct Location {
	int x, y;
	enum Type type;
	int cost, path;
	int out;
}Location;

Location map[ROW][COL];
Location computer, player;
int inCircle = 0;
const char *info[] = {"You win!", "You lost!"};
jmp_buf jmpBuf;

void init();
void init_map();
void draw_map();
void draw_one(enum Type type);
void get_left(Location cur, Location *get);
void get_leftUp(Location cur, Location *get);
void get_rightUp(Location cur, Location *get);
void get_right(Location cur, Location *get);
void get_rightDown(Location cur, Location *get);
void get_leftDown(Location cur, Location *get);
void get_round(Location cur, Location buf[]);
int is_boundary(Location cur);
void cal_onePath(Location *cur);
void go_player();
void go_computer();
int min_path(Location cur);
int max_cost(Location cur);
void cal_roundCost(Location cur);
void who_win(enum Who w);
int is_inCircle(Location cur);
void make_nextStep(Location *nextStep, int x, int y);
void cal_allPath();
void cal_allOut();
//void test();

int main()
{
_again:
	if(setjmp(jmpBuf) != 0)
		goto _again;
	init();
//	init_map();
//	test();
	while(1) {
		go_player();
		go_computer();
	}

	return 0;
}

void make_nextStep(Location *nextStep, int x, int y)
{
	nextStep->x = x;
	nextStep->y = y;
	nextStep->type = map[x][y].type;
	nextStep->cost = map[x][y].cost;
	nextStep->path = map[x][y].path;
	nextStep->out = map[x][y].out;
}

int is_inCircle(Location cur)
{
	cal_allOut();
	if(map[cur.x][cur.y].out == 0)
		return 0;
	else
		return 1;
}

void cal_roundCost(Location cur)
{
	Location buf[6], buf2[6];
	int i, j;
	
	get_round(cur, buf);
	for(i = 0; i < 6; i++)
		map[buf[i].x][buf[i].y].cost = 1;

	for(i = 0; i < 6; i++) {
		if(map[buf[i].x][buf[i].y].type != WAY) {
			continue;
		}
		else {
			get_round(buf[i], buf2); //note!
			for(j = 0; j < 6; j++) {
				if(map[buf2[j].x][buf2[j].y].type == WAY)
					map[buf[i].x][buf[i].y].cost++;
			}
		}
	}
}

int max_cost(Location cur)
{
	Location buf[6];
	int i, nextStep, maxCost = 0;

	cal_roundCost(cur);
	get_round(cur, buf);
	for(i = 0; i < 6; i++) 
		if((map[buf[i].x][buf[i].y].type == WAY) && 
				(maxCost < map[buf[i].x][buf[i].y].cost)) {
			maxCost = map[buf[i].x][buf[i].y].cost;
			nextStep = i;
		}
	if(maxCost == 0)
		who_win(PLAYER);

	return nextStep;
}

void go_computer()
{
	int i;
	int oldx = computer.x;
    int	oldy = computer.y;
	Location buf[6];

	if(is_boundary(computer))
		who_win(COMPUTER);

	if(is_inCircle(computer)) {
		i = max_cost(computer);
		inCircle = 1;
	}
	else 
		i = min_path(computer);

	switch(i) {
		case 0:
			computer.y--;
			break;
		case 1:
			if((computer.x % 2) == 0) {
				computer.x--;
				computer.y--;
			}
			else {
				computer.x--;
			}
			break;
		case 2:
			if((computer.x % 2) == 0) {
				computer.x--;				
			}
			else {
				computer.x--;
				computer.y++;
			}
			break;			
		case 3:
			computer.y++;
			break;
		case 4:
			if((computer.x % 2) == 0) {
				computer.x++;
			}
			else {
				computer.x++;
				computer.y++;
			}
			break;
		case 5:
			if((computer.x % 2) == 0) {
				computer.x++;
				computer.y--;
			}
			else {
				computer.x++;
			}
			break;
	} //end switch

	if((oldx % 2) == 0)
		move(oldx, oldy*2);
	else
		move(oldx, oldy*2+1);
	draw_one(WAY);
	map[oldx][oldy].type = WAY;

	if((computer.x % 2) == 0)
		move(computer.x, computer.y*2);
	else
		move(computer.x, computer.y*2+1);
	draw_one(CAT);
	map[computer.x][computer.y].type = CAT;
}

int min_path(Location cur)
{
	Location buf[6];
	int next_step = 0;
	int minPath;
	int i;

	cal_allPath();
	get_round(cur, buf);
	minPath = 100;
	for(i = 0; i < 6; i++) {
		if(buf[i].path < minPath) {
			next_step = i;
			minPath = buf[i].path;
		}
	}

	if(minPath == 100)
		who_win(PLAYER);

	return next_step;
}

void who_win(enum Who w)
{
	char ch;

	move(9, 0);
	if(w == COMPUTER)
		printw("%s", info[1]);
	else
		printw("%s", info[0]);
	refresh();
	move(10, 1);
	printw("try again?(y/n)");
	refresh();
	while(ch = getch()) {
		if((ch == 'y') || (ch == 'Y')){
			inCircle = 0;
			longjmp(jmpBuf, 1);
		}
		else if((ch == 'n') || (ch == 'N')) {
			move(11, 1);
			printw("You will quit the game in 2 seconds");
			refresh();
			sleep(2);
			endwin();
			exit(EXIT_SUCCESS);
		}
		else {
			move(11, 1);
			printw("please input y or n");
			refresh();
		}
	}
}

void go_player()
{
	int oldx, oldy;
	enum Type oldType;
	char ch;

	oldx = player.x;
	oldy = player.y;
	oldType = map[player.x][player.y].type;
	while(ch = getch()) {
		switch(ch) {
			case 'w':
			case 'W':
				if(player.x == 0)
					player.x = 8;
				else
					player.x--;
				break;
			case 's':
			case 'S':
				if(player.x == 8)
					player.x = 0;
				else
					player.x++;
				break;
			case 'a':
			case 'A':
				if(player.y == 0)
					player.y = 8;
				else
					player.y--;
				break;
			case 'd':
			case 'D':
				if(player.y == 8)
					player.y = 0;
				else
					player.y++;
				break;
			case ' ':
				if((player.x % 2) == 0)
					move(player.x, player.y*2);
				else
					move(player.x, player.y*2+1);
				if(map[player.x][player.y].type != CAT) {
					attron(A_STANDOUT);
					draw_one(STONE);
					attroff(A_STANDOUT);
					map[player.x][player.y].type = STONE;
				}
				break;
			default:
				break;
		}//end switch

		if(ch == ' ') {
			cal_allPath();
			cal_allOut();
			return;
		}

		if((oldx % 2) == 0)
			move(oldx, oldy*2);
		else
			move(oldx, oldy*2+1);
		draw_one(oldType);

		
		oldType = map[player.x][player.y].type;
		if((player.x % 2) == 0)
			move(player.x, player.y*2);
		else
			move(player.x, player.y*2+1);
		attron(A_STANDOUT);
		draw_one(oldType);
		attroff(A_STANDOUT);

		oldx = player.x;
		oldy = player.y;
	} //end while

}

int is_boundary(Location cur)
{
	int x = cur.x;
	int y = cur.y;

	if((x == 0) || (x == 8) || (y == 0) || (y == 8))
		return 1;
	return 0;
}

void get_left(Location cur, Location *get)
{
	//memcpy(get, &map[cur.x][cur.y-1], sizeof(Location));
	get->x = map[cur.x][cur.y-1].x;
	get->y = map[cur.x][cur.y-1].y;
	get->cost = map[cur.x][cur.y-1].cost;
	get->path = map[cur.x][cur.y-1].path;
	get->type = map[cur.x][cur.y-1].type;
	get->out = map[cur.x][cur.y-1].out;
}

void get_leftUp(Location cur, Location *get)
{
	if((cur.x % 2) == 0) {
		//memcpy(get, &map[cur.x-1][cur.y-1], sizeof(Location));
		get->x = map[cur.x-1][cur.y-1].x;
		get->y = map[cur.x-1][cur.y-1].y;
		get->cost = map[cur.x-1][cur.y-1].cost;
		get->path = map[cur.x-1][cur.y-1].path;
		get->type = map[cur.x-1][cur.y-1].type;
		get->out = map[cur.x-1][cur.y-1].out;
	}
	else {
		//memcpy(get, &map[cur.x-1][cur.y], sizeof(Location));
		get->x = map[cur.x-1][cur.y].x;
		get->y = map[cur.x-1][cur.y].y;
		get->cost = map[cur.x-1][cur.y].cost;
		get->path = map[cur.x-1][cur.y].path;
		get->type = map[cur.x-1][cur.y].type;
		get->out = map[cur.x-1][cur.y].out;
	}
}

void get_rightUp(Location cur, Location *get)
{
	if((cur.x % 2) == 0) {
		//memcpy(get, &map[cur.x-1][cur.y], sizeof(Location));
		get->x = map[cur.x-1][cur.y].x;
		get->y = map[cur.x-1][cur.y].y;
		get->cost = map[cur.x-1][cur.y].cost;
		get->path = map[cur.x-1][cur.y].path;
		get->type = map[cur.x-1][cur.y].type;
		get->out = map[cur.x-1][cur.y].out;
	}
	else {
		//memcpy(get, &map[cur.x-1][cur.y+1], sizeof(Location));
		get->x = map[cur.x-1][cur.y+1].x;
		get->y = map[cur.x-1][cur.y+1].y;
		get->cost = map[cur.x-1][cur.y+1].cost;
		get->path = map[cur.x-1][cur.y+1].path;
		get->type = map[cur.x-1][cur.y+1].type;
		get->out = map[cur.x-1][cur.y+1].out;
	}
}

void get_right(Location cur, Location *get)
{
//	memcpy(get, &map[cur.x][cur.y+1], sizeof(Location));
		get->x = map[cur.x][cur.y+1].x;
		get->y = map[cur.x][cur.y+1].y;
		get->cost = map[cur.x][cur.y+1].cost;
		get->path = map[cur.x][cur.y+1].path;
		get->type = map[cur.x][cur.y+1].type;
		get->out = map[cur.x][cur.y+1].out;
}

void get_rightDown(Location cur, Location *get)
{
	if((cur.x % 2) == 0) {
		//memcpy(get, &map[cur.x-1][cur.y], sizeof(Location));
		get->x = map[cur.x+1][cur.y].x;
		get->y = map[cur.x+1][cur.y].y;
		get->cost = map[cur.x+1][cur.y].cost;
		get->path = map[cur.x+1][cur.y].path;
		get->type = map[cur.x+1][cur.y].type;
		get->out = map[cur.x+1][cur.y].out;
	}
	else {
		//memcpy(get, &map[cur.x-1][cur.y+1], sizeof(Location));
		get->x = map[cur.x+1][cur.y+1].x;
		get->y = map[cur.x+1][cur.y+1].y;
		get->cost = map[cur.x+1][cur.y+1].cost;
		get->path = map[cur.x+1][cur.y+1].path;
		get->type = map[cur.x+1][cur.y+1].type;
		get->out = map[cur.x+1][cur.y+1].out;
	}
}

void get_leftDown(Location cur, Location *get)
{
	if((cur.x % 2) == 0) {
		//memcpy(get, &map[cur.x-1][cur.y-1], sizeof(Location));
		get->x = map[cur.x+1][cur.y-1].x;
		get->y = map[cur.x+1][cur.y-1].y;
		get->cost = map[cur.x+1][cur.y-1].cost;
		get->path = map[cur.x+1][cur.y-1].path;
		get->type = map[cur.x+1][cur.y-1].type;
		get->out = map[cur.x+1][cur.y-1].out;
	}
	else {
		//memcpy(get, &map[cur.x-1][cur.y], sizeof(Location));
		get->x = map[cur.x-1][cur.y].x;
		get->y = map[cur.x-1][cur.y].y;
		get->cost = map[cur.x-1][cur.y].cost;
		get->path = map[cur.x-1][cur.y].path;
		get->type = map[cur.x-1][cur.y].type;
		get->out = map[cur.x-1][cur.y].out;
	}
}

void get_round(Location cur, Location buf[])
{
	get_left(cur, &buf[0]);
	get_leftUp(cur, &buf[1]);
	get_rightUp(cur, &buf[2]);
	get_right(cur, &buf[3]);
	get_rightDown(cur, &buf[4]);
	get_leftDown(cur, &buf[5]);
}

void init_map()
{
	int i, j, k, x, y;

	for(i = 0; i < ROW; i++) {
		for(j = 0; j < COL; j++) {
			map[i][j].x = i;
			map[i][j].y = j;
			map[i][j].type = WAY;
			map[i][j].cost = 0;
			map[i][j].path = -100;
			map[i][j].out = -1;
		}
	}
	computer.x = 4;
	computer.y = 4;
	map[computer.x][computer.y].type = CAT;

	for(i = 0; i < INIT_STONE; i++) {
		x = rand() % ROW;
		y = rand() % COL;
		if((x == computer.x) && (y == computer.y)) 
			continue;
		map[x][y].type = STONE;
		map[x][y].path = 100;
	}
	player.x = 4;
	player.y = 5;
	player.type = map[player.x][player.y].type;

	//calulate all path
	for(i = 0, j = 0; i <= 4 ; i++, j++) {
		for(k = j; k <= (8-j); k++) {
			cal_onePath(&map[i][k]);
			cal_onePath(&map[8-i][k]);
		}
		for(k = i; k <= (8-i); k++) {
			cal_onePath(&map[k][j]);
			cal_onePath(&map[k][8-j]);
		}
	}

	cal_allOut();
}

void cal_allOut()
{
	int i, j, k, l, end;
	Location cur, buf[6];

	//init all out
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			map[i][j].out = -1;
		}
	}

	//init boundary out
	for(i = 0; i < 9; i++) {
		if(map[0][i].type != STONE)
			map[0][i].out = 0;
		if(map[8][i].type != STONE)
			map[8][i].out = 0;
		if(map[i][0].type != STONE)
			map[i][0].out = 0;
		if(map[i][8].type != STONE)
			map[i][8].out = 0;
	}
/*
	for(i = 1, j = 1, end = 7; i <= 4; i++, j++, end -= 2) {
		for(k = 0; k < end; k++) {
			if(map[i+k][j+k].type != STONE) {
				get_round(map[i+k][j+k], buf);
				for(l = 0; l < 6; l++) {
					if(map[buf[l].x][buf[l].y].out == 0) {
						map[i+k][j+k].out = 0;
						break;
					}
				}
			}
		}
	}
	for(i = 7, j = 7, end = 7; i >= 4; i--, j--, end -= 2) {
		for(k = 0; k < end; k++) {
			if(map[i-k][j-k].type != STONE) {
				get_round(map[i-k][j-k], buf);
				for(l = 0; l < 6; l++) {
					if(map[buf[l].x][buf[l].y].out == 0) {
						map[i-k][j-k].out = 0;
						break;
					}
				}
			}
		}
	}
	
*/
	//init other, (1,1)->(7,7)
	for(i = 1; i < 8; i++) {
		for(j = 1; j < 8; j++ ) {
			if(map[i][j].type != STONE) {
				get_round(map[i][j], buf);
				for(k = 0; k < 6; k++) {
					if(map[buf[k].x][buf[k].y].out == 0) {
						map[i][j].out = 0;
						break;
					}
				}									
			}
		}
	}

	//init other, (7,7)->(1,1)
	for(i = 7; i >= 0; i--) {
		for(j = 7; j >= 0; j--) {
			if((map[i][j].type != STONE) && (map[i][j].out == -1)) {
				get_round(map[i][j], buf);
				for(k = 0; k < 6; k++) {
					if(map[buf[k].x][buf[k].y].out == 0) {
						map[i][j].out = 0;
						break;
					}
				}
			}
		}
	}
	//init other, (7,1)->(1,7)
	for(i = 7; i >= 0; i--) {
		for(j = 1; j < 8; j++) {
			if((map[i][j].type != STONE) && (map[i][j].out == -1)) {
				get_round(map[i][j], buf);
				for(k = 0; k < 6; k++) {
					if(map[buf[k].x][buf[k].y].out == 0) {
						map[i][j].out = 0;
						break;
					}
				}
			}
		}
	}
	//init other, (1,7)->(7,1)
	for(i = 1; i < 8; i++) {
		for(j = 7; j >= 0; j--) {
			if((map[i][j].type != STONE) && (map[i][j].out == -1)) {
				get_round(map[i][j], buf);
				for(k = 0; k < 6; k++) {
					if(map[buf[k].x][buf[k].y].out == 0) {
						map[i][j].out = 0;
						break;
					}
				}
			}
		}
	}

}

void cal_allPath()
{
	int i, j, k;

	for(i = 0, j = 0; i <= 4 ; i++, j++) {
		for(k = j; k <= (8-j); k++) {
			cal_onePath(&map[i][k]);
			cal_onePath(&map[8-i][k]);
		}
		for(k = i; k <= (8-i); k++) {
			cal_onePath(&map[k][j]);
			cal_onePath(&map[k][8-j]);
		}
	}
}

void cal_onePath(Location *cur)
{
	int min, i;
	Location buf[6];

	if(cur->type == STONE)
		min = 100;
	 else {
		 if(is_boundary((*cur)))
			min =  0;
		else {
			get_round((*cur), buf);
			min = 100;
			for(i = 0; i < 6; i++) {
				if(min > abs(buf[i].path)) {
					min = buf[i].path;				
				}
			}
			if(min != 100)
				min++;
		}
	 }
	cur->path = min;
}

void draw_one(enum Type type)
{
	switch(type) {
		case WAY:
			addch(CWAY);
			break;
		case STONE:
			addch(CSTONE);
			break;
		case CAT:
			if(inCircle)
				addch(CCAT_IN_CIRCLE);
			else
				addch(CCAT);
			break;
		default:
			addch(' ');
			break;
	}
	refresh();
}

void draw_map()
{
	int i, j;

	for(i = 0; i < ROW; i++) {
		for(j = 0; j < COL; j++) {
			if((i % 2) == 0) {
				move(i, 2*j);
				draw_one(map[i][j].type);				
			}
			else {
				move(i, 2*j+1);
				draw_one(map[i][j].type);
			}
		}
	}
}

void init() 
{
	initscr();
	clear();
	refresh();
	cbreak();
	noecho();
	curs_set(0);
	srand(time(NULL));
	init_map();
	draw_map();
	inCircle = 0;
}

/*
void test()
{
	int i, j;
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			if((i % 2) == 0)
				printf("%3d    ",  map[i][j].path);
			else
				printf("    %3d", map[i][j].path);
		}
		printf("\n");
	}
}
*/
