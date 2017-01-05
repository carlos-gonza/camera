#ifndef TURNH
#define TURNH

#include <bcm2835.h>  // control gpio

#define PIN_1 RPI_BPLUS_GPIO_J8_07 // on is past initialized, also denotes left
#define PIN_2 RPI_BPLUS_GPIO_J8_11 // toggles when captured, also denotes right
#define PIN_3 RPI_BPLUS_GPIO_J8_13 // toggles when processed, also denotes start
#define PIN_4 RPI_BPLUS_GPIO_J8_15 // toggles when action received

#define PIN_A RPI_BPLUS_GPIO_J8_29
#define PIN_B RPI_BPLUS_GPIO_J8_31
#define PIN_C RPI_BPLUS_GPIO_J8_33
#define PIN_D RPI_BPLUS_GPIO_J8_35
#define PIN_E RPI_BPLUS_GPIO_J8_37

#ifndef TURN
extern void initturn();
extern void turnleft(int sleepfactor = 0);
extern void turnright(int sleepfactor = 0);
extern void start(int sleepfactor = 0);
extern void stop(int sleepfactor = 0);

extern void toggle1(int sleep = 0);
extern void toggle2(int sleep = 0);
extern void toggle3(int sleep = 0);
extern void toggle4(int sleep = 0);
extern void set1(bool bOn, int sleep = 0);
extern void set2(bool bOn, int sleep = 0);
extern void set3(bool bOn, int sleep = 0);
extern void set4(bool bOn, int sleep = 0);
extern void setall(bool bOn, int sleep = 0);

extern void toggleA(int sleep = 0);
extern void toggleB(int sleep = 0);
extern void toggleC(int sleep = 0);
extern void toggleD(int sleep = 0);
extern void toggleE(int sleep = 0);
extern void setA(bool bOn, int sleep = 0);
extern void setB(bool bOn, int sleep = 0);
extern void setC(bool bOn, int sleep = 0);
extern void setD(bool bOn, int sleep = 0);
extern void setE(bool bOn, int sleep = 0);
#endif
#endif
