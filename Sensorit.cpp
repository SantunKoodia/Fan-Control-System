/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#include "Sensors.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include <cstdio>

// TODO: insert other definitions and declarations here
static volatile bool adcdone = false;
static volatile bool adcstart = false;
#define TICKRATE_HZ (100)	/* 100 ticks per second */

extern "C" {

void SysTick_Handler(void)
{
	static uint32_t count;

	/* Every 1/2 second */
	count++;
	if (count >= (TICKRATE_HZ * 2)) {
		count = 0;
		adcstart = true;
		Board_LED_Toggle(1);
	}
}

void ADC0A_IRQHandler(void)
{
	uint32_t pending;

	/* Get pending interrupts */
	pending = Chip_ADC_GetFlags(LPC_ADC0);

	/* Sequence A completion interrupt */
	if (pending & ADC_FLAGS_SEQA_INT_MASK) {
		adcdone = true;
	}

	/* Clear any pending interrupts */
	Chip_ADC_ClearFlags(LPC_ADC0, pending);
}

} // extern "C"


int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
	Board_LED_Set(1, true);
#endif
#endif

	Sensors sensor;

	/* Enable related ADC NVIC interrupts */
	NVIC_EnableIRQ(ADC0_SEQA_IRQn);

	/* Enable sequencer */
	Chip_ADC_EnableSequencer(LPC_ADC0, ADC_SEQA_IDX);

	/* Configure systick timer */
	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ);

	uint32_t a0, a3;


	while(1) {
		while(!adcstart) __WFI();
		adcstart = false;

		Chip_ADC_StartSequencer(LPC_ADC0, ADC_SEQA_IDX);
		while(!adcdone) __WFI();
		adcdone = false;
		a0 = Chip_ADC_GetDataReg(LPC_ADC0, 0);
		a3 = Chip_ADC_GetDataReg(LPC_ADC0, 3);
		sensor.set_co2(ADC_DR_RESULT(a0));
		sensor.set_temp(ADC_DR_RESULT(a3));
		printf("CO2 = %d temp = %d\n", sensor.get_co2(), sensor.get_temp());

	}

	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;
	}

	return 0 ;
}
