#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdbool.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "Buzzer.h"
#include "KeyPad.h"
#include "Sens.h"
#include "LCD.h"

#define CYCLE_DURATION 						(1)
#define DEFAULT_CODE(_CharArray)			{int i; for(i=0; i<CODE_DIM; i++){_CharArray[i]=(i+49);} _CharArray[CODE_DIM]='\0';}

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
	int pwm_freq;
}AlarmStruct;

void
alarm_init(AlarmStruct *state){
	int i;
	for(i=0; i<NUMBER_OF_SENSE+1; i++){
		state -> eventsArray[i]=false;
	}
	state -> isActive = false;
}

void change_pin(char code[], KeyStruct * keyPadState){
	bool isEquals = true;
	int i =0;

	keypad_flush(keyPadState);
	HD44780_ClrScr();
	HD44780_GotoXY(0,0);
	HD44780_PutStr("OLD PIN:");
	HD44780_GotoXY(0,1);

	while(keyPadState->index != CODE_DIM){
		get_code(keyPadState);
	}

	while(keyPadState->code[i] != '\0' && code[i] != '\0'){
		(isEquals & (keyPadState->code[i] == code[i])) ? (isEquals = true) : (isEquals = false);
		i++;
	}
	keypad_flush(keyPadState);


	if(!isEquals){
		HD44780_PutStr("ERRATO");
		return;
	}

	HD44780_ClrScr();
	HD44780_GotoXY(0,0);
	HD44780_PutStr("NEW PIN:");

	while(keyPadState->index != CODE_DIM)
			get_code(keyPadState);

	for(i =0; i<CODE_DIM+1; i++)
		code[i] = keyPadState->code[i];
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


#endif
