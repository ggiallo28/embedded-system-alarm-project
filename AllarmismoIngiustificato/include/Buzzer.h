#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32f30x.h"
#include "stdbool.h"
#include "Sens.h"

// ----------------------------------------------------------------------------

// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#define BUZZER_PORT_NUMBER               (3)
#define BUZZER_PIN_NUMBER                (15)

#define BUZZER_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BUZZER_PIN_MASK(_N)              (1 << (_N))
#define BUZZER_RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))

#define BUZZER_TIMER	TIM4
#define BUZZER_TIMER_CHANNEL_INIT	TIM_OC4Init
#define BUZZER_TIMER_CHANNEL_PRELOAD_CONFIG TIM_OC4PreloadConfig
#define BUZZER_TIMER_CHANNEL_SET_COMPARE	TIM_SetCompare4

#define BUZZER_TIMER_AF	GPIO_AF_2

#define BUZZER_TIMER_RCC_BIT RCC_APB1Periph_TIM4

#define BUZZER_TIMER_CLOCK_FREQUENCY 			(720000)
#define BUZZER_DUTY_CYCLE 						(0.5)
#define BUZZER_MAX_FREQ       					(16000)
#define BUZZER_MIN_FREQ       					(100)
#define LOWUP									(1)
#define UPLOW									(-1)

#define BUZZER_x100INCREMENT					(2)
// ----------------------------------------------------------------------------
typedef struct {
	bool eventsArray[NUMBER_OF_SENSE+1];
	bool isActive;
	bool isRinging;
	int pwm_freq;
}AlarmStruct;


void buzzer_init(void);
inline void alarm_on(AlarmStruct *);
inline void alarm_off(AlarmStruct *);

// ----------------------------------------------------------------------------
inline void
__attribute__((always_inline))
alarm_on(AlarmStruct * state){
	int increment = (state->pwm_freq/100)*BUZZER_x100INCREMENT;
	int direction = LOWUP;

	if(state->pwm_freq>BUZZER_MAX_FREQ)
		direction = UPLOW;
	if(state->pwm_freq<BUZZER_MIN_FREQ)
		direction = LOWUP;

	state->pwm_freq+=(increment*direction);

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	uint32_t prescaler = RCC_Clocks.PCLK2_Frequency / BUZZER_TIMER_CLOCK_FREQUENCY - 1;
	uint32_t period = BUZZER_TIMER_CLOCK_FREQUENCY / state->pwm_freq;
	TIM_TimeBaseStructure.TIM_Period = period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BUZZER_TIMER, &TIM_TimeBaseStructure);

	BUZZER_TIMER_CHANNEL_SET_COMPARE(BUZZER_TIMER, period * BUZZER_DUTY_CYCLE);
}

inline void
__attribute__((always_inline))
alarm_off(AlarmStruct * state) {
	BUZZER_TIMER_CHANNEL_SET_COMPARE(BUZZER_TIMER, 0);
	state->pwm_freq = BUZZER_MIN_FREQ;
}

#endif
