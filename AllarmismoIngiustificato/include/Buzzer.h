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

#define FREQUENCY								(1000)

#define MAX_TIME								(200)
#define DO_FREQ									(262)


// ----------------------------------------------------------------------------

void buzzer_init(void);
void alarm_on(void);
inline void alarm_off(void);
inline void pin_sound(int);

// ---------------------------------------------------------------------------

inline void
__attribute__((always_inline))
alarm_off() {
	BUZZER_TIMER_CHANNEL_SET_COMPARE(BUZZER_TIMER, 0);
}

inline void
__attribute__((always_inline))
pin_sound(int pwm_freq) {
	BUZZER_TIMER_CHANNEL_SET_COMPARE(BUZZER_TIMER, BUZZER_TIMER_CLOCK_FREQUENCY / pwm_freq);
}

#endif
