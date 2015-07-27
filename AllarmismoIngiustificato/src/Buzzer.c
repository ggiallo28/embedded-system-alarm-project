//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "stm32f30x_tim.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "Buzzer.h"


// ----------------------------------------------------------------------------

void buzzer_init() {

	//////////////// SET PIN ///////////////////////

	RCC_AHBPeriphClockCmd(BUZZER_RCC_MASKx(BUZZER_PORT_NUMBER), ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN_MASK(BUZZER_PIN_NUMBER);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BUZZER_GPIOx(BUZZER_PORT_NUMBER), &GPIO_InitStructure);
	GPIO_PinAFConfig(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_NUMBER, BUZZER_TIMER_AF);
	//////////////// FINE SET PIN //////////////////////

	///////////////// SET TIMER //////////////////////

	// Enable Timer clock
	RCC_APB1PeriphClockCmd(BUZZER_TIMER_RCC_BIT, ENABLE);

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	uint32_t prescaler = RCC_Clocks.PCLK2_Frequency / BUZZER_TIMER_CLOCK_FREQUENCY - 1;
	uint32_t period = BUZZER_TIMER_CLOCK_FREQUENCY 	/ DO_FREQ;
	TIM_TimeBaseStructure.TIM_Period = period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BUZZER_TIMER, &TIM_TimeBaseStructure);
	///////////////// FINE SET TIMER /////////////////////

	//////////////// SET CANALE /////////////////////

	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //currCount < dutycicle ? (uscita=1) : (uscita=0)
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;

	BUZZER_TIMER_CHANNEL_INIT(BUZZER_TIMER, &TIM_OCInitStructure);
	BUZZER_TIMER_CHANNEL_PRELOAD_CONFIG(BUZZER_TIMER, TIM_OCPreload_Enable);
	///////////// FINE SET CANALE /////////////////
	TIM_Cmd(BUZZER_TIMER, ENABLE);
}
/*********************************************************************************************
 *@Param:		- void
 *@return:      - void
 *Descrizione	: Suona il buzzer a due frequenze differenti per simulare un'allarme.
 *********************************************************************************************/
void
alarm_on(){
	static int index =0;
	static int time =0;
	int pwm_freq;


	if((time = (time+1)%MAX_TIME)  == 0)
		index = (index+1)%2;

	pwm_freq=(index+1)*FREQUENCY;


	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	uint32_t prescaler = RCC_Clocks.PCLK2_Frequency / BUZZER_TIMER_CLOCK_FREQUENCY - 1;
	uint32_t period = BUZZER_TIMER_CLOCK_FREQUENCY / pwm_freq;
	TIM_TimeBaseStructure.TIM_Period = period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BUZZER_TIMER, &TIM_TimeBaseStructure);

	BUZZER_TIMER_CHANNEL_SET_COMPARE(BUZZER_TIMER, period * BUZZER_DUTY_CYCLE);
}


// ----------------------------------------------------------------------------


