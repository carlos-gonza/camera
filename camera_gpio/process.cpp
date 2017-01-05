#define  PROCESS
#include "process.h"
#include "turn.h"
#include "state.h"
 
extern bool doTest;
extern bool doVerbose;
extern bool doDumpAction;
extern int BmpNum;
extern bool togglesenabled;
extern bool SetActionFStatsAndState(Action &a, FrameStats &fs, State &s, int width);
extern bool GetFrameStats(BYTE* pmatrix, int width, int height, int firstrow, FrameStats &fs);
extern void DumpRequest();
extern int findfirstroadrow(BYTE *pmatrix, int width, int height);

 /**
 * bool processframe(Frame &f, Action &a)
 * process frame f and return if it dictates
 * action necessary, in which case action
 * is returned by parameter Action &a
 * 
 */
 bool processframe(Frame &f, Action &a)
 {
	 bool bActionNeeded = false;
	 FrameStats fs;
	 State s;
	 static int firstrow = 0;
	 
	 // analyze f.Grid
	 BYTE *pmatrix = (BYTE *) &f.data[0];
	 if (firstrow == 0) {
	 	firstrow = findfirstroadrow(pmatrix,f.imageWidth, f.imageHeight);
	 }
	 	
	 GetFrameStats(pmatrix, f.imageWidth, f.imageHeight, firstrow, fs);
	 bActionNeeded = SetActionFStatsAndState(a, fs, s, f.imageWidth);
	 if (doTest) 
		printf("processframe: bActionNeeded = %d\n", bActionNeeded);
	 a.captureticks = f.captureticks;

	return bActionNeeded;
 }
 
 void LogAction(Action a)
 {
 	static FILE *fp = NULL;

 	if (fp == NULL) {
 		fp = fopen("/home/pi/usbdrv/projects/camera_gpio/actionlog.txt", "a");
 		printf("fp = %x", fp);
 	}

 	if (fp != NULL) {
	 	if (a.aa & RIGHT) {
	 		fprintf(fp, "%d R\n", BmpNum);
	 	}
	 	if (a.aa & LEFT) {
	 		fprintf(fp, "%d L\n", BmpNum);
	 	}
	 	if (a.aa & STOP) {
	 		fprintf(fp, "%d STOP\n", BmpNum);
	 	}
	 	if (a.aa & START) {
	 		fprintf(fp, "%d START\n", BmpNum);
	 	}
	 	fclose(fp);
	 	fp = NULL;
	 }
 }
/**
 * processqueue
 * pop from the frameque fq and process recent frames only
 * processed frames requiring an action are pushed to the action queue aq
 * 
 * 
 */
void processthread()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::milliseconds;	
	unsigned int count = 0;
	unsigned int skippedcount = 0;
	unsigned int actioncount = 0;
	printf("processthread enterd\n");
	while (!bexitprocess) {
		if (!fq.empty()) {
			count++;
			if (count % 100 == 0){
				//printf("frame q size = %d non empty = %d skipped = %d added to action = %d\n", fq.size(), count, skippedcount, actioncount);
				
				skippedcount = 0;
				count = 0;
				actioncount = 0;
			}
			Frame f = fq.front();
			fq.pop();
						
					//SaveBitmapToFile( (BYTE*) &f.data[0], f.imageWidth, f.imageHeight, 24, 0, "pri_image.bmp" ); 
			clock_t now = clock();
			if ((now - f.captureticks) < CAPTURETICKS_THRESHHOLD) {
				// frame was captured recently so process it
				Action a;
				bool bTakeAction = processframe(f, a);
				if (togglesenabled)
					toggle3();
				if (bTakeAction) {
					aq.push(a);
					actioncount++;
					if (doDumpAction) {
					    DumpRequest();
					    LogAction(a);					
						BmpNum++;  // increment for next image to be saved
						if (doVerbose)
							printf("dumped action image number %d\n", BmpNum-1);
						if (BmpNum == 100) {
							doDumpAction = false;
							if (doVerbose)
								printf("disabled dumping action images..\n");
						}
						
					}
				}
			} else {
				skippedcount++;
			}
		} else
			std::this_thread::sleep_for(milliseconds(10));
	}
	printf("processthread exiting\n");
}
