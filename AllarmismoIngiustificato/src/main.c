#include "main.h"

int
main(int argc, char* argv[])
{
	trace_puts("Hello ARM World!");
	trace_printf("System clock: %uHz\n", SystemCoreClock);

/************** VARIABLE DECLARATION */
	KeyStruct keyPadState;
	AlarmStruct state;
	EdgeStruct sense;
	char defaultCode[PIN_DIM+1];
	bool isEquals;

/************* INIT ***************/
	keypad_init(&keyPadState);
	timer_start();
	sense_init(&sense);
	alarm_init(&state,defaultCode);
	lcd_init();
	buzzer_init();


/************** INFINITE LOOP *****/
	HD44780_ClrScr();
	HD44780_GotoXY(0,0);
	HD44780_PutStr("SALVE PF");
	HD44780_GotoXY(0,1);
	HD44780_PutStr("FOGGIA");
	keypad_flush(&keyPadState);

	 while(1){
		 state.eventsArray[MAGN1] = magn_one_read(&sense);
		 state.eventsArray[MAGN2] = magn_two_read(&sense);
		 state.eventsArray[MOVE1] = move_one_read(&sense);
		 state.eventsArray[MOVE2] = move_two_read(&sense);
		 state.eventsArray[MOVE3] = move_three_read(&sense);
		 state.eventsArray[MOVE4] = move_four_read(&sense);
		 state.eventsArray[KEYPAD] = get_code(&keyPadState);

/******** Update State ***********/
		isEquals = true;

		if(state.eventsArray[KEYPAD]){
		int i=0;
		HD44780_ClrScr();
		HD44780_GotoXY(0,0);
		HD44780_PutStr("PIN:");
		HD44780_GotoXY(0,1);
		while(keyPadState.pin[i] != '\0' && defaultCode[i] != '\0'){
			HD44780_PutChar(keyPadState.pin[i]);
			(isEquals & (keyPadState.pin[i] == defaultCode[i])) ? (isEquals = true) : (isEquals = false);
			i++;
		}


		if(isEquals){
			state.isActive = !state.isActive;
			keypad_flush(&keyPadState);

			if(state.isActive){
				HD44780_ClrScr();
				HD44780_GotoXY(0,0);
				HD44780_PutStr("ALLARME");
				HD44780_GotoXY(0,1);
				HD44780_PutStr("INSERITO");
			}else{
				HD44780_ClrScr();
				HD44780_GotoXY(0,0);
				HD44780_PutStr("ALLARME");
				HD44780_GotoXY(0,1);
				HD44780_PutStr("INIBITO");
			}
		}
		if(!isEquals && keyPadState.index == PIN_DIM){
			HD44780_GotoXY(0,1);
			HD44780_PutStr("ERRATO");
			keypad_flush(&keyPadState);
		}
	}
 /******** Generate Outputs *******/
		 if(state.eventsArray[MAGN1] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("PORTA 1");
		 }
		 if(state.eventsArray[MAGN2] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("PORTA 2");
		 }
		 if(state.eventsArray[MOVE1] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("STANZA 1");
		 }
		 if(state.eventsArray[MOVE2] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("STANZA 2");
		 }
		 if(state.eventsArray[MOVE3] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("STANZA 3");
		 }
		 if(state.eventsArray[MOVE4] && state.isActive){
			 state.isRinging = true;
			 HD44780_ClrScr();
			 HD44780_GotoXY(0,0);
			 HD44780_PutStr("EVENTO");
			 HD44780_GotoXY(0,1);
			 HD44780_PutStr("STANZA 4");
		 }

		 if(!state.isActive){
			 alarm_off(&state);
			 state.isRinging = false;
		 }

		 if(state.isRinging)
			 alarm_on(&state);
	 }
}

void
alarm_init(AlarmStruct *state, char *defaultCode){
	int i;
	for(i=0; i<NUMBER_OF_SENSE+1; i++){
		state -> eventsArray[i]=false;
	}
	state -> isActive = false;
	DEFAULT_CODE(defaultCode);
	state->pwm_freq[0] = FREQ1;
	state->pwm_freq[1] = FREQ2;
	state->time =0;
	state->index
}

// ----------------------------------------------------------------------------

