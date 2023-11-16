//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);
void myADC_Init(void);
void myDAC_Init(void);

uint32_t edge = 0x0;
uint32_t count = 0x0000;
uint32_t analog_pot = 0x0;
uint32_t digital_pot = 0x0;
int pot_value = 0;
int dac_value = 0;
float period = 0;
float frequency = 0;
int source = 1;

// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/


int
main(int argc, char* argv[])
{

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */
	myADC_Init();   /* Initialize ADC/DAC */
	myDAC_Init();

	while (1)
	{

		if((ADC1->ISR & 0x00000001) != 0)
		{

			ADC1->CR |= ADC_CR_ADSTART;
			while((ADC1->ISR & 0x4) == 0);
			//analog_pot = ADC1->DR;
			//pot_value = analog_pot/1;
			//digital_pot = DAC->DHR12R1;
			//dac_value = analog_pot/1;
			//trace_printf("ADC DR %d %d\n", pot_value, dac_value);

		}

		if((GPIOA->IDR & GPIO_IDR_0) != 0)
		{
			/* Wait for button to be released (PA0 = 0) */
			while((GPIOA->IDR & GPIO_IDR_0) != 0); // should be changed to interrupt at some point
			if(source == 1)
				source = 0;
			else
				source = 1;
		}
	}

	return 0;

}



void myGPIOA_Init() // *WRITTEN*
{
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA0 as input */
	// Relevant register: GPIOA->MODER
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);


	//GPIOA->MODER |= ((uint32_t)0xF00);
	//GPIOA->PUPDR = ((uint32_t)0x00000000);


	/* Ensure no pull-up/pull-down for PA0 */ //user button
	// Relevant register: GPIOA->PUPDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

	/* Configure PA1 as input */
	// Relevant register: GPIOA->MODER
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);

	/* Ensure no pull-up/pull-down for PA1 */ // timer
	// Relevant register: GPIOA->PUPDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	/* Configure PA2 as input */
	// Relevant register: GPIOA->MODER
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);

	/* Ensure no pull-up/pull-down for PA2 */ // function gen
	// Relevant register: GPIOA->PUPDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

	/* Configure PA5 as analog */
	// Relevant register: GPIOA->MODER
	GPIOA->MODER |= (GPIO_MODER_MODER5);

	/* Ensure no pull-up/pull-down for PA5 */ // pot
	// Relevant register: GPIOA->PUPDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

	/* Configure PA4 as analog */
	// Relevant register: GPIOA->MODER
	GPIOA->MODER |= (GPIO_MODER_MODER4);

	// 00000000000000000000111100000000
}


void myTIM2_Init() // *WRITTEN*
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	// Relevant register: TIM2->CR1
	TIM2->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM2->EGR = ((uint16_t)0x0001);

	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
	TIM2->DIER |= TIM_DIER_UIE;
}


void myEXTI_Init()
{
	/* Map EXTI2 line to PA2, EXTI1 line to PA1 */
	SYSCFG->EXTICR[0] = ((uint32_t)0x00000000);
	//
	/* EXTI2 and EXTI1 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= ((uint32_t)0x00000006); // assumes that 555 timer signal behaves the same as function generator

	/* Unmask interrupts from EXTI2, EXTI1 line */
	// Relevant register: EXTI->IMR
	EXTI->IMR |= ((uint32_t)0x00000006);

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	/* Enable EXTI2 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI2_3_IRQn);

	/* Assign EXTI1 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	/* Enable EXTI1 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);



}

void myADC_Init()
{

	/* Enable ADC clock and SPI */
	RCC->APB2ENR = ((uint32_t)0x00001200);


	/* Set data resolution to 12 bits, right align data, overwrite overuns,
	   set continuous conversion mode */
	ADC1->CFGR1 = ((uint32_t)0x00003000);

	/* Clear CR before enabling ADC */
	ADC1->CR = ((uint32_t)0x00000000);

	/* ADC enable */
	ADC1->CR = ((uint32_t)0x00000001);


	/* Set sampling rate to 239 1/2 ADC clock cycles */
	ADC1->SMPR = ((uint32_t)0x00000007);

	/* Select port 5 for conversion */
	ADC1->CHSELR = ((uint32_t)0x00000020);

	/* Clear ISR */
	//ADC1->ISR = ((uint32_t)0x00000000);


}

void myDAC_Init()
{

	/* Enable DAC clock */
	RCC->APB1ENR = RCC_APB1ENR_DACEN;

	/* Enable DAC Channel 1*/
	DAC->CR = DAC_CR_EN1;

}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR
		NVIC->ICER[0] = ((uint32_t)0x0000000);

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN;

	}
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler() //we might have to stay in this handler while we wait for the next interrupt(edge)
{ //we might also have to use event updates to count how many edges
	// Declare/initialize your local variables here...

	/* Check if EXTI2 interrupt pending flag is indeed set */
	if (((EXTI->PR & EXTI_PR_PR2) && source) != 0)
	{
		//
		// 1. If this is the first edge:
		if(edge == 0x0){
			edge = 0x1; //set the local count variable to 1

		//	- Clear count register (TIM2->CNT).
		TIM2->CNT = ((uint32_t)0x00000000);

		//	- Start timer (TIM2->CR1).
		TIM2->CR1 |= TIM_CR1_CEN;

		//    Else (this is the second edge):
		} else if (edge == 0x1) {
		TIM2->CR1 &= ~TIM_CR1_CEN;
		//	- Stop timer (TIM2->CR1).

		//	- Read out count register (TIM2->CNT).
		count = TIM2->CNT;

		// set the edge counter to 0
			edge = 0;
		//	- Calculate signal period and frequency.
		//	- Print calculated values to the console.
		//	  NOTE: Function trace_printf does not work
		//	  with floating-point numbers: you must use
		//	  "unsigned int" type to print your signal
		//	  period and frequency.

		period = count/48000.;
		frequency = 1./period;

		trace_printf("\nGenerator - Period: %f, Frequency: %f\n", period, frequency);
		//trace_printf("\ngenerator count: %u\n", count);

		//	- Clear count register (TIM2->CNT).
		TIM2->CNT = ((uint32_t)0x00000000);

		//	- Start timer (TIM2->CR1).
		TIM2->CR1 |= TIM_CR1_CEN;
		}
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		//
		/* Clear update interrupt flag */ //sorta bit from one
		//EXTI->PR  = EXTI_PR_PR2;
		TIM2->CR1 |= TIM_CR1_CEN;	/* Restart stopped timer */ //sorta bit from one
	}


	EXTI->PR = EXTI_PR_PR2;

}

void EXTI0_1_IRQHandler() { //value generation is taken from 2_3 handler

	if (((EXTI->PR & EXTI_PR_PR1) && !source) != 0){

			//
			// 1. If this is the first edge:
			if(edge == 0x0){
				edge = 0x1; //set the local count variable to 1

			//	- Clear count register (TIM2->CNT).
			TIM2->CNT = ((uint32_t)0x00000000);

			//	- Start timer (TIM2->CR1).
			TIM2->CR1 |= TIM_CR1_CEN;

			//    Else (this is the second edge):
			} else if (edge == 0x1) {
			TIM2->CR1 &= ~TIM_CR1_CEN;
			//	- Stop timer (TIM2->CR1).

			//	- Read out count register (TIM2->CNT).
			count = TIM2->CNT;

			// set the edge counter to 0
			edge = 0;
			//	- Calculate signal period and frequency.
			//	- Print calculated values to the console.
			//	  NOTE: Function trace_printf does not work
			//	  with floating-point numbers: you must use
			//	  "unsigned int" type to print your signal
			//	  period and frequency.

			period = count/48000.;
			frequency = 1./period;

			//trace_printf("\nTimer - Period: %f, Frequency: %f\n", period, frequency);
			trace_printf("\ntimer count: %u\n", count);

			//	- Clear count register (TIM2->CNT).
			TIM2->CNT = ((uint32_t)0x00000000);

			//	- Start timer (TIM2->CR1).
			TIM2->CR1 |= TIM_CR1_CEN;
			//EXTI->PR = EXTI_PR_PR1;

		}
	}

	EXTI->PR = EXTI_PR_PR1;

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
