#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32f30x.h"
#include "stdbool.h"
#include "Sens.h"

// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#define BUZZER_PORT_NUMBER               (3)
#define BUZZER_PIN_NUMBER                (6)
#define BUZZER_ACTIVE_LOW                (0)

#define BUZZER_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BUZZER_PIN_MASK(_N)              (1 << (_N))
#define BUZZER_RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))

// ----------------------------------------------------------------------------

extern void buzzer_init(void);
inline void alarm_on(void);
inline void alarm_off(void);

// ----------------------------------------------------------------------------
inline void
__attribute__((always_inline))
alarm_on(void)
{
#if (BUZZER_ACTIVE_LOW)
    GPIO_ResetBits(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_MASK(BUZZER_PIN_NUMBER));
#else
    GPIO_SetBits(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_MASK(BUZZER_PIN_NUMBER));
#endif
}

inline void
__attribute__((always_inline))
alarm_off(void)
{
#if (BUZZER_ACTIVE_LOW)
    GPIO_SetBits(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_MASK(BUZZER_PIN_NUMBER));
#else
    GPIO_ResetBits(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_MASK(BUZZER_PIN_NUMBER));
#endif
}

// ----------------------------------------------------------------------------

#endif // BUZZER_H_
