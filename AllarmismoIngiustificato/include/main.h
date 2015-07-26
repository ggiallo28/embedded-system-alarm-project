#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "Buzzer.h"
#include "KeyPad.h"
#include "Sens.h"
#include "LCD.h"

#define CYCLE_DURATION 						(1)
#define DEFAULT_CODE(_CharArray)			{int i; for(i=0; i<PIN_DIM; i++){_CharArray[i]=(i+49);} _CharArray[PIN_DIM]='\0';}

#define MAGN1 								(0)
#define MAGN2 								(1)
#define MOVE1 								(2)
#define MOVE2 								(3)
#define MOVE3 								(4)
#define MOVE4 								(5)
#define KEYPAD 								(6)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
typedef struct {
	bool eventsArray[NUMBER_OF_SENSE+1];
	bool isActive;
	bool isRinging;
}AlarmStruct;

void
alarm_init(AlarmStruct *state){
	int i;
	for(i=0; i<NUMBER_OF_SENSE+1; i++){
		state -> eventsArray[i]=false;
	}
	state -> isActive = false;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


#endif
