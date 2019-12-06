/*
 * led.c
 *
 *  Created on: 18-Nov-2019
 *      Author: Devansh
 */

#include "led.h"
#include "gpio.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "log.h"

void set_duty_cycle(int duty_cycle)
{
	TIMER_CompareBufSet(TIMER0, 0, TIMER_TopGet(TIMER0) * duty_cycle);
}

void TIMER0_IRQHandler(void)
{
  uint32_t compareValue;

  /* Clear flag for TIMER0 overflow interrupt */
  TIMER_IntClear(TIMER0, TIMER_IF_OF);

  compareValue = TIMER_CaptureGet(TIMER0, 0);
  /* increment duty-cycle or reset if reached TOP value */
  if( compareValue == TIMER_TopGet(TIMER0))
    TIMER_CompareBufSet(TIMER0, 0, 0);
  else
    TIMER_CompareBufSet(TIMER0, 0, ++compareValue);
}

void timer_pwm()
{
	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Select CC channel parameters */
	TIMER_InitCC_TypeDef timerCCInit =
	{
		.eventCtrl  = timerEventEveryEdge,
		.edge       = timerEdgeBoth,
		.prsSel     = timerPRSSELCh0,
		.cufoa      = timerOutputActionNone,
		.cofoa      = timerOutputActionNone,
		.cmoa       = timerOutputActionToggle,
		.mode       = timerCCModePWM,
		.filter     = false,
		.prsInput   = false,
		.coist      = false,
		.outInvert  = false,
	};

	/* Configure CC channel 0 */
	TIMER_InitCC(TIMER0, 0, &timerCCInit);

	TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
	TIMER0->ROUTELOC0 |= _TIMER_ROUTELOC0_CC0LOC_LOC18;

	/* Set Top Value */
	TIMER_TopSet(TIMER0, (CMU_ClockFreqGet(cmuClock_HFPER)/10000));

	LOG_INFO("Timer value: %lu", (CMU_ClockFreqGet(cmuClock_HFPER)/10000));

	/* Set compare value starting at 0 - it will be incremented in the interrupt handler */
	//TIMER_CompareBufSet(TIMER0, 0, (CMU_ClockFreqGet(cmuClock_HFPER)/100000));
	TIMER_CompareBufSet(TIMER0, 0, 0);

	/* Select timer parameters */
	TIMER_Init_TypeDef timerInit =
	{
		.enable     = true,
		.debugRun   = true,
		.prescale   = timerPrescale64,
		.clkSel     = timerClkSelHFPerClk,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode       = timerModeUp,
		.dmaClrAct  = false,
		.quadModeX4 = false,
		.oneShot    = false,
		.sync       = false,
	};

	/* Enable overflow interrupt */
	//TIMER_IntEnable(TIMER0, TIMER_IF_OF);

	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	/* Configure timer */
	TIMER_Init(TIMER0, &timerInit);

	//gpioBuzzer(1);
}
