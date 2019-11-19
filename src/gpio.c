/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include <string.h>
#include "event.h"
#include "native_gecko.h"

#define	LED0_port gpioPortF
#define LED0_pin	4
#define LED1_port gpioPortF
#define LED1_pin 5

void gpioInit()
{
	/* Configure PB0 and PB1 as input */
	GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, 1);
	GPIO_PinModeSet(PB1_port, PB1_pin, gpioModeInput, 1);
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void gpioEnableDisplay()
{
	GPIO_PinOutSet(SENSOR_ENABLE_port, SENSOR_ENABLE_pin);
}

void gpioSetDisplayExtcomin(bool high)
{
	high ? GPIO_PinOutSet(EXTCOMIN_port, EXTCOMIN_pin) : GPIO_PinOutClear(EXTCOMIN_port, EXTCOMIN_pin);
}
