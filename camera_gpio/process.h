#ifndef PROCESSH
#define PROCESSH

#include <chrono>
#include <time.h>
#include <cstdio>
#include "frame.h"
#include "action.h"

#ifndef  PROCESS
extern bool bexitprocess;
extern int CAPTURETICKS_THRESHHOLD; 
#else
bool bexitprocess;
int CAPTURETICKS_THRESHHOLD = CLOCKS_PER_SEC/10; // tenth of a second
#endif
void processthread();
#endif
