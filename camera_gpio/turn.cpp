#define TURN
#include "turn.h"

void initturn()
{
	bcm2835_gpio_fsel(PIN_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_A, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_B, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_C, BCM2835_GPIO_FSEL_OUTP);	
	bcm2835_gpio_fsel(PIN_D, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_E, BCM2835_GPIO_FSEL_OUTP);		
	
	bcm2835_gpio_write(PIN_1, LOW);
	bcm2835_gpio_write(PIN_2, LOW);
	bcm2835_gpio_write(PIN_3, LOW);
	bcm2835_gpio_write(PIN_4, LOW);
	bcm2835_gpio_write(PIN_A, LOW);
	bcm2835_gpio_write(PIN_B, LOW);
	bcm2835_gpio_write(PIN_C, LOW);
	bcm2835_gpio_write(PIN_D, LOW);
	bcm2835_gpio_write(PIN_E, LOW);	
}



static bool p1 = false;	// camera initalized
static bool p2 = false;	// toggles on capture
static bool p3 = false;	// toggles on processed
static bool p4 = false;	// toggles on action pop

static bool A = false;
static bool B = false;
static bool C = false;	
static bool D = false;	
static bool E = false;

void set1(bool bOn, int sleep = 0)
{
	p1 = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_1, HIGH);
	else
		bcm2835_gpio_write(PIN_1, LOW);
	if (sleep > 0)
		delay(sleep);
}

void set2(bool bOn, int sleep = 0)
{
	p2 = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_2, HIGH);
	else
		bcm2835_gpio_write(PIN_2, LOW);
	if (sleep > 0)
		delay(sleep);
}

void set3(bool bOn, int sleep = 0)
{
	p3 = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_3, HIGH);
	else
		bcm2835_gpio_write(PIN_3, LOW);
	if (sleep > 0)
		delay(sleep);
}

void set4(bool bOn, int sleep = 0)
{
	p4 = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_4, HIGH);
	else
		bcm2835_gpio_write(PIN_4, LOW);
	if (sleep > 0)
		delay(sleep);
}

void setall(bool bOn, int sleep = 0)
{
	  set1(bOn, sleep);
	  set2(bOn, sleep);
	  set3(bOn, sleep);
	  set4(bOn, sleep);
}

void setA(bool bOn, int sleep = 0)
{
	A = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_A, HIGH);
	else
		bcm2835_gpio_write(PIN_A, LOW);
	if (sleep > 0)
		delay(sleep);
}

void setB(bool bOn, int sleep = 0)
{
	B = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_B, HIGH);
	else
		bcm2835_gpio_write(PIN_B, LOW);
	if (sleep > 0)
		delay(sleep);	
}

void setC(bool bOn, int sleep = 0)
{
	C = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_C, HIGH);
	else
		bcm2835_gpio_write(PIN_C, LOW);
	if (sleep > 0)
		delay(sleep);		
}

void setD(bool bOn, int sleep = 0)
{
	D = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_D, HIGH);
	else
		bcm2835_gpio_write(PIN_D, LOW);
	if (sleep > 0)
		delay(sleep);		
}

void setE(bool bOn, int sleep = 0)
{
	E = bOn;
	if (bOn)
		bcm2835_gpio_write(PIN_E, HIGH);
	else
		bcm2835_gpio_write(PIN_E, LOW);
	if (sleep > 0)
		delay(sleep);		
}

void toggle1(int sleep)
{
	set1(p1 ? false : true, sleep);
}

void toggle2(int sleep = 0)
{
	set2(p2 ? false : true, sleep);
}

void toggle3(int sleep = 0)
{
	set3(p3 ? false : true, sleep);
}

void toggle4(int sleep = 0)
{
	set4(p4 ? false : true, sleep);
}

void toggleA(int sleep = 0)
{
	setA(A ? false : true, sleep);
}

void toggleB(int sleep = 0)
{
	setB(B ? false : true, sleep);
}

void toggleC(int sleep = 0)
{
	setC(C ? false : true, sleep);
}

void toggleD(int sleep = 0)
{
	setD(D ? false : true, sleep);
}

void toggleE(int sleep = 0)
{
	setE(E ? false : true, sleep);
}

void turnleft(int sleepfactor = 0)
{
	set1(true, sleepfactor*100);
	set1(false);
}

void turnright(int sleepfactor = 0)
{
	set2(true, sleepfactor*100);
	set2(false);
}

void start(int sleepfactor = 0)
{
	set3(true, sleepfactor*100);
	set3(false);
}

void stop(int sleepfactor = 0)
{
	set4(true, sleepfactor*100);
	set4(false);
}
