#ifndef BUTTONH
#define BUTTONH

#include <bcm2835.h>  // control gpio

#define BUTTON_1 RPI_GPIO_P1_12
#define BUTTON_2 RPI_GPIO_P1_16
#define BUTTON_3 RPI_GPIO_P1_18
#define BUTTON_4 RPI_GPIO_P1_22


#ifndef BUTTON
extern void initbutton();
extern bool isApressed(); // in test mode, press to trigger a capture simulation
extern bool isBpressed(); // press to exit all threads
extern bool isCpressed(); // initially pins toggle on capture, process, and action. 
						  // press to toggle the toggles (see togglesenabled)
extern bool isDpressed(); // press to toggle verbose (default is off)
#endif
#endif
