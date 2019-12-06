/*****************************************************************************
​ ​* ​ ​ @file​ ​  		letimer.c
​ * ​ ​ @brief​ ​ 		source file to initialize letimer and enable interrupts.
					Interrupt handler toggles led based on period and on time
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 11th, 2019
​ * ​ ​ @version​ ​ 		2.0
*****************************************************************************/

#include "letimer.h"
#include "em_letimer.h"
#include "gpio.h"
#include "event.h"
#include "em_core.h"
#include "log.h"
#include "main.h"
#include "display.h"

void timerEnable1HzSchedulerEvent(void)
{
	LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_UF)); //Enable underflow interrupt only when display is initialized
}

/*
 * @func - LETIMER0_IRQHandler
 * @brief - Toggle LED and clear interrupts
 * @parameters - none
 * @return - none
 */
void LETIMER0_IRQHandler(void)
{
	uint32_t int_flag;

	int_flag = LETIMER_IntGet(LETIMER0);

	//LOG_INFO("Interrupt flag: %x; TimeCntr: %x\n",int_flag,TimeStamp_count);

	/* 1 second interrupt */
	if(int_flag & LETIMER_IEN_UF)
	{
		/* Clear interrupts */
		LETIMER_IntClear(LETIMER0, (LETIMER_IEN_UF));
		TimeStamp_count++;
		gecko_external_signal(DISPLAY_UPDATE);
	}
}

/*
 * @func - sleepBlockStart
 * @brief - prevent sleeping from the energy mode specified
 * @parameters - SLEEP_EnergyMode_t - sleep mode (and lower) to be blocked
 * @return - none
 */
void sleepBlockStart(SLEEP_EnergyMode_t sleep_mode_blocked)
{
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	SLEEP_SleepBlockBegin(sleep_mode_blocked);
	CORE_EXIT_CRITICAL();
}

/*
 * @func - sleepBlockEnd
 * @brief - stop blocking system from sleeping from the energy mode specified
 * @parameters - SLEEP_EnergyMode_t - sleep mode (and lower) to be unblocked
 * @return - none
 */
void sleepBlockEnd(SLEEP_EnergyMode_t sleep_mode_blocked)
{
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	SLEEP_SleepBlockEnd(sleep_mode_blocked);
	CORE_EXIT_CRITICAL();
}

/*
 * @func - letimerInit
 * @brief - Initialize letimer and enable interrupts
 * @parameters - int - comp0 value
 * 				 int - comp1 value
 * 				 SLEEP_EnergyMode_t - sleep mode (and lower) to be blocked
 * @return - none
 */
void letimerInit(int comp0, int comp1, SLEEP_EnergyMode_t sleep_mode_blocked)
{
	TimeStamp_count = 0;

	const LETIMER_Init_TypeDef LETIMER_CONFIG = {
			true,				//Start counting when init complete
			false,				//debugRun - Don't run when debug halt
			true,				//comp0top - load comp0 into CNT when counter underflows
			false,				//buftop - Don't load COMP1 into COMP0
			0,					//idle value for output 0
			0, 					//idle value for output 1
			letimerUFOANone,    //No underflow output 0 action
			letimerUFOANone,	//No underflow output 1 action
			letimerRepeatFree,	//Repeat mode - none
	};

	//LETIMER0->ROUTELOC0 |= LETIMER_ROUTELOC0_OUT0LOC_LOC18;
	//LETIMER0->ROUTEPEN |= LETIMER_ROUTEPEN_OUT0PEN;

	/* Repetition values must be nonzero so that the outputs
 	 return switch between idle and active state */
	/*LETIMER_RepeatSet(LETIMER0, 0, 0x01);
	LETIMER_RepeatSet(LETIMER0, 1, 0x01);*/

	LETIMER_Init(LETIMER0, &LETIMER_CONFIG);
	LETIMER_CompareSet(LETIMER0, 0, comp0);
	LETIMER_CompareSet(LETIMER0, 1, 0xFFFF); //Set it with the highest value to prevent comp1 flag from being set

	//LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_UF)); //Interrupt when under flow
	NVIC_EnableIRQ(LETIMER0_IRQn);
	LETIMER_Enable(LETIMER0, true);
}


