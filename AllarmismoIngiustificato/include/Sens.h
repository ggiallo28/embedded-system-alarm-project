#ifndef SENS_H_
#define  SENS_H_

#include "stm32f30x.h"
#include <stdbool.h>

// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#define SENSE_PORT_NUMBER              	   (2)

#define MAGN_ONE_PIN_NUMBER                (15)
#define MAGN_TWO_PIN_NUMBER                (11)

#define MOVE_ONE_PIN_NUMBER                 (13)
#define MOVE_TWO_PIN_NUMBER 				(14)
#define MOVE_THREE_PIN_NUMBER               (10)
#define MOVE_FOUR_PIN_NUMBER                (12)


#define NUMBER_OF_SENSE 					(6)
#define MAX_DIM 							(16)
#define SENSE_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define SENSE_PIN_MASK(_N)              (1 << (_N))
#define SENSE_RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))
// ----------------------------------------------------------------------------

typedef struct{
	int prevStates[MAX_DIM];
}EdgeStruct;

extern
void
sense_init(EdgeStruct*);
void move_init();
void magn_init();

int sense_read(int);

void edge_detection_init(EdgeStruct*);

int edge_read(EdgeStruct*, int);

inline int
__attribute__((always_inline))
magn_one_read(EdgeStruct* s)
{
	return edge_read(s,MAGN_ONE_PIN_NUMBER);
}

inline int
__attribute__((always_inline))
magn_two_read(EdgeStruct* s)
{
	return edge_read(s,MAGN_TWO_PIN_NUMBER);
}

inline int
__attribute__((always_inline))
move_one_read(EdgeStruct* s)
{
	return edge_read(s,MOVE_ONE_PIN_NUMBER);
}

inline int
__attribute__((always_inline))
move_two_read(EdgeStruct* s)
{
	return edge_read(s,MOVE_TWO_PIN_NUMBER);
}
inline int
__attribute__((always_inline))
move_three_read(EdgeStruct* s)
{
	return edge_read(s,MOVE_THREE_PIN_NUMBER);
}
inline int
__attribute__((always_inline))
move_four_read(EdgeStruct* s)
{
	return edge_read(s,MOVE_FOUR_PIN_NUMBER);
}


// ----------------------------------------------------------------------------

#endif
