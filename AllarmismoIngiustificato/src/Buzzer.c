#include "Buzzer.h"
#include "main.h"

// ----------------------------------------------------------------------------

void
buzzer_init()
{

  RCC_AHBPeriphClockCmd(BUZZER_RCC_MASKx(BUZZER_PORT_NUMBER), ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = BUZZER_PIN_MASK(BUZZER_PIN_NUMBER);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BUZZER_GPIOx(BUZZER_PORT_NUMBER), &GPIO_InitStructure);

  alarm_off();
}
