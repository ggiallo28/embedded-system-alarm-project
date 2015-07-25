/*
 * MagneticSensor.c
 *
 *  Created on: 09/lug/2015
 *      Author: Gianluigi
 */

#include "Sens.h"

// ----------------------------------------------------------------------------

void
sense_init( EdgeStruct* edge){
	move_init();
	magn_init();
	edge_detection_init(edge);
}

void edge_detection_init(EdgeStruct *s){
	int i=0;
	for(i=0;i<MAX_DIM;i++){
		s->prevStates[i]=0;
	}

}

/*
 * Questo metodo ci è utile per individuare il fronte di salita dell'uscita di un sensore. Ci serve questa cosa per evitare che il sensore continui
 * a triggare subito dopo aver variato il suo stato.
 */
int edge_read(EdgeStruct *s, int pin){
	int lecture = sense_read(pin);
	if(lecture == s->prevStates[pin]){
		return 0;
	}

	if(s->prevStates[pin] == 0 && lecture==1){ //fronte di salita
		s->prevStates[pin]=lecture;
		return 1;
	}else{
		s->prevStates[pin]=lecture;
		return 0;
	}

}

void
magn_init(){
	  // Enable GPIO Peripheral clock
	  RCC_AHBPeriphClockCmd(SENSE_RCC_MASKx(SENSE_PORT_NUMBER), ENABLE);

	  GPIO_InitTypeDef GPIO_InitStructure;

	  // Configure pin in output push/pull mode
	  GPIO_InitStructure.GPIO_Pin = ( SENSE_PIN_MASK(MAGN_ONE_PIN_NUMBER)  |
			  	  	  	  	  	  	  SENSE_PIN_MASK(MAGN_TWO_PIN_NUMBER)  );

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_Init(SENSE_GPIOx(SENSE_PORT_NUMBER), &GPIO_InitStructure);
}

void
move_init()
{
  RCC_AHBPeriphClockCmd(SENSE_RCC_MASKx(SENSE_PORT_NUMBER), ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = ( SENSE_PIN_MASK(MOVE_ONE_PIN_NUMBER  	) |
		  	  	  	  	  	  	  SENSE_PIN_MASK(MOVE_TWO_PIN_NUMBER  	) |
								  SENSE_PIN_MASK(MOVE_THREE_PIN_NUMBER	) |
								  SENSE_PIN_MASK(MOVE_FOUR_PIN_NUMBER  	) );

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(SENSE_GPIOx(SENSE_PORT_NUMBER), &GPIO_InitStructure);
}

// ----------------------------------------------------------------------------
int sense_read(int pin_number){
	unsigned data = GPIO_ReadInputData((SENSE_GPIOx(SENSE_PORT_NUMBER)));
	int toReturn = ((data & SENSE_PIN_MASK(pin_number)) != 0);
	return toReturn;
}


