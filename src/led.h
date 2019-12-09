/*
 * led.h
 *
 * Corresponding header file for led.c
 *
 *  Created on: 18-Nov-2019
 *      Author: Devansh
 */

#ifndef SRC_LED_H_
#define SRC_LED_H_

#include <stdint.h>

void test_led(void);
void timer_pwm();
void set_duty_cycle(uint8_t duty_cycle);

#endif /* SRC_LED_H_ */
