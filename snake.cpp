#include <iostream>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sys/select.h>
#include <stdio.h>
#include <cmath>
#include <curses.h>
#include <cstdlib>
#include <unordered_map>
#include <time.h>
#include <chrono>

using namespace std;

enum direction { UP = 10, DOWN = 11, LEFT = 12, RIGHT = 13};
enum states { RUNNING = 0, PAUSED = 1, STOPPED = 2 };

static unordered_map<int, string> strings;

static int map[20][20] = {};
static int snk[800] = {};
static int snkHead = 0;
static int snkTail = 0;
static int snkLen;
static int snDirection;
static int state = STOPPED;

void drawHLine() {
	printw("+");
	for(int i = 0; i < 20; i++)	{
		printw("--");
	}
	printw("+\n");
}

void getPlInput() {
	int key = getch();	
	if( key == 27) {
		getch();
		key = getch();
	}

	switch(key) {
		case 'w':
			snDirection = UP;
			break;
		case 's':
			snDirection = DOWN;
			break;
		case 'a':
			snDirection = LEFT;
			break;
		case 'd':
			snDirection = RIGHT;
			break;
		case 'p':
			state = state ^ PAUSED;
			break;
		case 'x':
			state = STOPPED;
			break;
	}
}

void addFood() {
	int x, y;

	x = rand() % 20;
	y = rand() % 20;
	
	while(true) {
		
		for(int i = snkTail; i <= snkHead; i++) {
			if(x == snk[i*2]) {
				x = rand() % 20;
				continue;
			}
			if(y == snk[i*2 + 1]) {
				y = rand() % 20;
				continue;
			}
		}
		break;
	}
	map[x][y] = 1;
}

void initWorld() {

	strings.insert(make_pair(0,"RUNNING"));
	strings.insert(make_pair(1,"PAUSED"));
	strings.insert(make_pair(2,"STOPPED"));
	strings.insert(make_pair(10,"UP"));
	strings.insert(make_pair(11,"DOWN"));
	strings.insert(make_pair(12,"LEFT"));
	strings.insert(make_pair(13,"RIGHT"));

	//snk = {};
	snDirection = 10 + (rand() % 4);

	/* start center*/
	snkHead = 0;
	snkTail = 0;
	snkLen = 1;
	snk[0] = 10;
	snk[1] = 10;
	map[10][10] = 1;

	addFood();

	state = RUNNING;
}

void moveSn(int x, int y) {
	snkHead++;
	snkTail++;
	int i = snkHead * 2;
	snk[i] = x;
	snk[i + 1] = y;
}

void growSn(int x, int y) {
	snkHead++;
	int i = snkHead * 2;
	snk[i] = x;
	snk[i + 1] = y;
	snkLen++;
}

void updateSn() {
	int x = snk[snkHead * 2];
	int y = snk[snkHead * 2 + 1];
	
	int x1 = snk[snkTail * 2];
	int y1 = snk[snkTail * 2 + 1];

	switch(snDirection) {
		case UP:
			y--;
			break;
		case DOWN:
			y++;
			break;
		case LEFT:
			x--;
			break;
		case RIGHT:
			x++;
			break;
	}	

	if(x < 0 || x > 20 || y < 0 || y > 20) {
		state = STOPPED;
		return;
	}
	for(int i = snkTail; i <= snkHead; i++) {
		if(x == snk[i * 2] && y == snk[i * 2 + 1]) {
			state = STOPPED;
			return;
		}
	}

	if(map[x][y]) {
		growSn(x,y);
		addFood();
	} else {
		moveSn(x, y);
		map[x1][y1] = 0;
	}
	map[x][y]=1;
}

void updateWorld() {
	updateSn();

	// for(int i = snkHead; i < snkLen; i++) {
	// 	map[i*2][i*2+1] = 1;
	// }
}

void renderWorld() {
	clear();
	drawHLine();

	for(int i = 0; i < 20; i++) {
		printw("|");
		for(int j = 0; j < 20; j++) {
			printw("%2s", (map[j][i] == 0) ? " " : "*");
		}
		printw("|\n");
	}

	drawHLine();

	/* Debug  */
	printw("Direction:%s\n", strings[snDirection].c_str());
	printw("x:%d y:%d \n", snk[snkHead * 2], snk[snkHead * 2 +1]);
	printw("state:%s \n", strings[state].c_str());
	printw("snake:\n\n");
	printw("[");
	for(int i = snkTail; i <= snkHead; i++) {
		printw("(%d,%d),", snk[i*2], snk[i*2+1]);
	}
	printw("]");
}

void renderScreenGG() {
	clear();
	printw("\n");
	printw("\n  ____  ____ ");
 	printw("\n/ ___ |/ ___|");
	printw("\n| |  _| |  _ ");
	printw("\n| |_| | |_| |");
 	printw("\n\\____|\\____|");
 	printw("\n\n bilg (c) 2018.");
	refresh();
}

long getMilliSeconds() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

int main(int argc, char** argv) {
	initWorld();
	
	int timeDelta = 0;
	long t1,t2;

	WINDOW *w = initscr();			/* Start curses mode 		  */
	nodelay(w, true);

	t1 = getMilliSeconds();
	while( state != STOPPED ) {
		usleep(100000);		
		renderWorld();
		printw("timeDelta:%d t1:%d t2:%d\n", timeDelta, t1, t2);
		refresh();			/* Print it on to the real screen */

		getPlInput();
		if (state == PAUSED) continue;
		
		t2 = getMilliSeconds();
		timeDelta = t2 - t1;
		if(timeDelta < 800) continue;
		
		updateWorld();
		t1 = getMilliSeconds();
	}
	renderScreenGG();
	nodelay(w, false);
	getch();
	endwin();			/* End curses mode		  */
	return 0;
}





/*

state = STOPPED;

*/