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
	pin_sound();
	bool isEquals = true;

	keypad_flush(keyPadState);
	HD44780_ClrScr();
	HD44780_GotoXY(0,0);
	HD44780_PutStr("OLD PIN:");
	HD44780_GotoXY(0,1);

	alarm_off();

	int i =0;
	while(keyPadState->index != CODE_DIM){
		if(get_code(keyPadState)){
			pin_sound();
			HD44780_GotoXY(0,1);
			while(i<CODE_DIM){
				HD44780_PutChar(keyPadState->code[i]);
				i++;
			}
			timer_sleep(2);
			alarm_off();
			i=0;
		}
	}

	i=0;
	while(keyPadState->code[i] != '\0' && code[i] != '\0'){
		(isEquals & (keyPadState->code[i] == code[i])) ? (isEquals = true) : (isEquals = false);
		i++;
	}
	keypad_flush(keyPadState);

	if(!isEquals){
		HD44780_PutStr("ERRATO");
		return;
	}
	bool confirm = false;

	while(!confirm){
		HD44780_ClrScr();
		HD44780_GotoXY(0,0);
		HD44780_PutStr("NEW PIN:");
		i=0;
		while(keyPadState->index != CODE_DIM){
			if(get_code(keyPadState)){
				pin_sound();
				HD44780_GotoXY(0,1);
				while(i<CODE_DIM){
					HD44780_PutChar(keyPadState->code[i]);
					i++;
				}
				timer_sleep(2);
				alarm_off();
				i=0;
			}
		}
		HD44780_GotoXY(0,0);
		HD44780_PutStr("CONFIRM?");
		HD44780_GotoXY(0,1);
		HD44780_PutStr("Y=* N=#");


		keypad_read(keyPadState);
		while(keyPadState->prevChar != ENTER_CHAR && keyPadState->prevChar != CLEAR_CHAR ){
			keypad_read(keyPadState);
		}
		if(keyPadState->prevChar== ENTER_CHAR){
			confirm=true;
			for(i =0; i<CODE_DIM+1; i++)
				code[i] = keyPadState->code[i];
		}
		else if(keyPadState->prevChar == CLEAR_CHAR)
			confirm=false;
		keypad_flush(keyPadState);
		keyPadState->prevChar = '\0';
	}

	HD44780_ClrScr();
	HD44780_GotoXY(1,0);
	HD44780_PutStr("WAIT..");
	timer_sleep(1000);
	while(keyPadState->prevChar == ENTER_CHAR)
		keyPadState->prevChar = '\0';

	HD44780_GotoXY(0,1);
	HD44780_PutStr("INSERITO");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


#endif
