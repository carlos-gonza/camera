#define BUTTON
#include "button.h"

void initbutton()
{
	bcm2835_gpio_fsel(BUTTON_1,  BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(BUTTON_2,  BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(BUTTON_3,  BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(BUTTON_4,  BCM2835_GPIO_FSEL_INPT);
}

bool isApressed()
{
	return (LOW == bcm2835_gpio_lev(BUTTON_1));
}

bool isBpressed()
{
	return (LOW == bcm2835_gpio_lev(BUTTON_2));
}

bool isCpressed()
{
	return (LOW == bcm2835_gpio_lev(BUTTON_3));
}

bool isDpressed()
{
	return (LOW == bcm2835_gpio_lev(BUTTON_4));
}

