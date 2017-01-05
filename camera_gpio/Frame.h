#ifndef FRAMEH
#define FRAMEH

#include "types.h"
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <queue>
#include <vector>

#define VAL_IGNORE 180
#define DELTA 30
#define VERT 6
#define HORIZ 6
#define RANGEL 400
#define RANGEH 400

typedef struct tagFRAME {
	//RGB Grid[512][512];
	BYTE data[1024*1024*3]; // change to 512*512*3
	int imageWidth;
	int imageHeight;
	clock_t captureticks;
	clock_t processticks;
} Frame;

typedef struct tagFrameStats {
	bool bleftmarker;
	bool brightmarker;
	int leftmarker;
	int rightmarker;
	int markerswidth;
	bool carinfront;
	int cardistance;
	int carapproachrate;
} FrameStats;

#ifndef  FRAME
extern std::queue<Frame> fq;
#else
std::queue<Frame> fq;
#endif
#endif
