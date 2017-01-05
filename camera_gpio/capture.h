
#ifndef CAPTUREH
#define CAPTUREH
#include <chrono>
#include <queue>
#include "raspicam.h"
#include "bmp.h"
#include <thread>
#include "frame.h"

using namespace std;

#ifndef CAPTURE
extern bool bexitcapture;
#else
bool bexitcapture;
#endif

void capturethread(raspicam::RaspiCam *pCamera);
#endif
