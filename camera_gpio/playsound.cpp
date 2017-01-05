#define PLAYSOUND
#include "playsound.h"

void playsoundthread(AnAction aa)
{
	if (aa & STOP) {
		system("mpg321 -q -a bluetooth -g 50 stop.mp3");
	}
	if (aa & START) {
		system("mpg321 -q -a bluetooth -g 50 start.mp3");
	}
	if (aa & LEFT) {
		system("mpg321 -q -a bluetooth -g 50 left.mp3");
	}
	if (aa & RIGHT) {
		system("mpg321 -q -a bluetooth -g 50 right.mp3");
	}
	if (aa & FAST) {
		system("mpg321 -q -a bluetooth -g 50 fast.mp3");
	}
}
