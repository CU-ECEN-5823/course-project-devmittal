/*
 * event.h
 *
 * brief: Contains function definitions and macros related to event.c
 *
 *  Created on: 14-Sep-2019
 *      Author: Devansh
 */

#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#include <stdint.h>

/* events */
enum My_Events {
	TURN_SI7021_ON = 1,
	DELAY_DONE,
	I2C_TRANSFER_DONE = 4,
	DISPLAY_UPDATE = 8,
};

#endif /* SRC_EVENT_H_ */
