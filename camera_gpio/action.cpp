#define ACTION
#include "action.h"
#include "process.h"
#include "playsound.h"
#include "turn.h"
#include <thread>

extern bool doTest;
extern bool doVerbose;
extern bool togglesenabled;


/**
 * doaction
 *
 */
void doaction(Action a)
{
	if (togglesenabled)
		toggle4();
	if (a.aa & LEFT) {
		if (doVerbose)
			printf("actionthread: LEFT\n");
		if (a.bplaysound) {
			std::thread pst (playsoundthread, LEFT);
			pst.detach();
		}
		if (!togglesenabled)
			turnleft(1);
	}
	if (a.aa & RIGHT) {
		if (doVerbose)
			printf("actionthread: RIGHT\n");
		if (a.bplaysound) {
			std::thread pst (playsoundthread, RIGHT);
			pst.detach();
		}
		if (!togglesenabled)
			turnright(1);
	}
	if (a.aa & FAST) {
		if (doVerbose)
			printf("actionthread: FAST\n");
		if (a.bplaysound) {
			std::thread pst (playsoundthread, FAST);
			pst.detach();
		}
	}
	if (a.aa & START) {
		if (doVerbose)
			printf("actionthread: START\n");
		if (a.bplaysound) {
			std::thread pst (playsoundthread, START);
			pst.detach();
		}
		if (!togglesenabled)
			start(1);
	}
	if (a.aa & STOP) {
		if (doVerbose)
			printf("actionthread: STOP\n");
		if (a.bplaysound) {
			std::thread pst (playsoundthread, STOP);
			pst.detach();
		}
		if (!togglesenabled)
			stop(1);
	}
}

/**
 * actionthread
 * pop from the action queue aq and take action on recent elements only
 * 
 */
void actionthread()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::milliseconds;	
	Action a;
	unsigned long countelementsactedon = 0;
	unsigned long countelements = 0;

	printf("actionthread entered\n");
	
	while(!bexitaction) {
		if (!aq.empty()) {
			a = aq.front();
			aq.pop();
			clock_t now = clock();
			if ((now - a.captureticks) < 10*CAPTURETICKS_THRESHHOLD) {
				doaction(a);
				countelementsactedon++;
			}
			countelements++;
		} else
			std::this_thread::sleep_for(milliseconds(10));
	}
	printf("actionthread exiting\n");
}