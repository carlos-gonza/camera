#ifndef ACTIONH
#define ACTIONH

#include <chrono>
#include "types.h"
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <queue>

typedef enum tagANACTION {
	NOTHING = 0,
	LEFT = 1,
	RIGHT = 2,
	START = 4,
	STOP = 8,
	SLOW = 16,
	FAST = 32
} AnAction;

typedef struct tagACTION {
	long aa; 				// a set of AnAction's
	bool bplaysound; 		// play sound if true
	clock_t captureticks; 	// ticks at frame captured
	clock_t processticks; 	// ticks at frame processed
} Action;

#ifndef  ACTION
extern bool bexitaction;
extern std::queue<Action> aq;
#else
std::queue<Action> aq;
bool bexitaction;
#endif
void actionthread();
#endif
