#include "LCD.h"
#include "stm32f30x_tim.h"
/*@Param:       - none.
 *@return:      - void.
 *
 *Richiama tutte le funzioni di inizializzazione.
 */
void lcd_init(){
	HD44780_Init();
	pwm_init();
	HD44780_set_contrast(20);
}

void HD44780_Init(void){
	GPIO_InitTypeDef        GPIO_InitStructure;
	RCC_ClocksTypeDef 		RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);

	RCC_AHBPeriphClockCmd(LCD_RCC_MASKx(HD44780_DATAPORT), ENABLE);

	GPIO_InitStructure.GPIO_Pin = (	LCD_PIN_MASK(HD44780_DATABIT7) |
			LCD_PIN_MASK(HD44780_DATABIT6) |
			LCD_PIN_MASK(HD44780_DATABIT5) |
			LCD_PIN_MASK(HD44780_DATABIT4) |
			LCD_PIN_MASK(HD44780_DATABIT3) |
			LCD_PIN_MASK(HD44780_DATABIT2) |
			LCD_PIN_MASK(HD44780_DATABIT1) |
			LCD_PIN_MASK(HD44780_DATABIT0) );

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_GPIOx(HD44780_DATAPORT), &GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(LCD_RCC_MASKx(HD44780_CONTROLPORT), ENABLE);

	GPIO_InitStructure.GPIO_Pin = (	LCD_PIN_MASK(HD44780_EN_BIT) |
			LCD_PIN_MASK(HD44780_RW_BIT) |
			LCD_PIN_MASK(HD44780_RS_BIT) );

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_GPIOx(HD44780_CONTROLPORT), &GPIO_InitStructure);

	hd44780_EN_Off();															 		/* Clear BIT di controllo. EN BIT di abilitazione del LCD. 		  													*/
	hd44780_RS_Off();																	/* Register Select. 0:  Instruction register. 1:  Data register. 													*/
	hd44780_RW_Off();																	/* Selects read or write. 0:  Write 1:  Read 																		*/

	hd44780_init_delay(); 														 		/* Funzioni per l'inizializzazione del lcd, bisogna attendere un certo tempo affinchè il comando venga eseguito 	*/
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );
	hd44780_init_delay2();
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );
	hd44780_init_delay3();
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );

	hd44780_function( HD44780_CONF_BUS, HD44780_CONF_LINES, HD44780_CONF_FONT );        /* Si settano i Parametri di configurazione 																		*/

	hd44780_display( HD44780_DISP_ON, HD44780_DISP_CURS_OFF, HD44780_DISP_BLINK_OFF );	/* Si accende il dispaly disabilitando la visualizzazione del cursore ed il blinking 								*/

	hd44780_clear();																	/* Si inizializza la memoria del LCD 																				*/

	hd44780_entry( HD44780_ENTRY_ADDR_INC, HD44780_ENTRY_SHIFT_CURS );					/* Si setta l'incremento, cioe' di quanti indirizzi di memoria bisogna spostare il cursore in avanti ad ogni
																						   inserimento di un carattere																						*/
}
/*@Param: 	-none
 *@return 	-void
 *
 *Funzione di inizializzazione del PWM utilizzato per controllare il contrasto del LCD.
 */
void pwm_init(){
	//////////////// SET PIN ///////////////////////

	RCC_AHBPeriphClockCmd(LCD_RCC_MASKx(CONTRAST_PORT_NUMBER), ENABLE);					/* Abilitiamo il clock alla porta 																					*/

	GPIO_InitTypeDef GPIO_InitStructure;												/* Struttura di configurazione 																						*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;										/* Settiamo il PIN per funzionare in modalità Alternativa sulla base di quanto presente sul datasheet 				*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_MASK(CONTRAST_PIN_NUMBER);					/* Scegliamo il PIN da utilizzare 																					*/
	GPIO_Init(LCD_GPIOx(CONTRAST_PORT_NUMBER), &GPIO_InitStructure);					/* Abilitiamo il PIN 																								*/
	GPIO_PinAFConfig(LCD_GPIOx(CONTRAST_PORT_NUMBER), CONTRAST_PIN_NUMBER, TIMER_AF);	/* Abilitiamo la funzione alternativa sul PIN 																		*/
	//////////////// FINE SET PIN //////////////////////

	///////////////// SET TIMER //////////////////////
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;										/* Strutture per la configurazione del TIMER usato per realizzare il PWM 											*/
	RCC_ClocksTypeDef RCC_Clocks;
	TIM_OCInitTypeDef TIM_OCInitStructure;												/* Struttura per la configurazione del canale 																		*/
	uint32_t prescaler=0;																/* Variabile per la configurazione del Prescaler 																	*/

	RCC_APB2PeriphClockCmd(TIMER_RCC_BIT, ENABLE);										/* Abilitiamo il clock al TIMER 																					*/

	RCC_GetClocksFreq(&RCC_Clocks);														/* Configurazione del TIMER 																						*/
	prescaler=RCC_Clocks.PCLK2_Frequency/TIMER_CLOCK_FREQUENCY-1;						/* Settiamo la frquenza del clock del prescaler, conteremo quindi +1 ogni TIMER_CLOCK_FREQUENCY colpi di clock      */
	TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD-1;									/* Periodo del TIMER, valore fino al quale bisogna contare prima del azzeramento    								*/
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;									/* Si imposta il prescaler 																							*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;								/* Un'ulteriore divisione del clock, in questo caso TIM_CKD_DIV1 indica di non dividere 							*/
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;							/* Direzione del conteggio 																							*/
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;									/* Per quante volte ripetiamo il conteggio, con 0 indichiamo che si riprende sempre da capo 						*/
	TIM_TimeBaseInit(TIMER, &TIM_TimeBaseStructure);									/* Si Inizializza il prescaler ed il contatore 																		*/
	///////////////// FINE SET TIMER /////////////////////

	//////////////// SET CANALE /////////////////////
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;									/* Modalità del PWM, quella vista a lezione 																		*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;						/* */
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;							/* Consente di scegliere se vogliamo che il segnale di output sia alto in corrispondenza del valore logico alto del */
																						/* PWM o invertito. Noi scegliamo la prima opzione																	*/
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
																						/* Inizializzazione dei Canali */
	TIMER_CHANNEL1_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL1_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL2_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL2_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL3_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL3_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL4_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL4_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	HD44780_set_contrast(0);

	TIM_CtrlPWMOutputs(TIMER, ENABLE);
	TIM_Cmd(TIMER, ENABLE);
}

/********************************************************************************************
 *@Param: 		- uint16_t: settaggio del comparatore.
 *@return:  	- void
 *
 *Descrizione 	: Funzione che permette di settare il comparatore.
 *				  Fintantoche' il valore a cui siamo arrivati a contontare è < in uscita dal
 *				  comparatore abbiamo un 1, altrimenti abbiamo uno 0. ??????????????????????
 *******************************************************************************************/
void
HD44780_set_contrast(uint16_t value) {
	TIMER_CHANNEL_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL2_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL3_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL4_SET_COMPARE(TIMER, value);
}
/*********************************************************************************************
 *@Param:		- char*: Puntatore alla stringa che deve essere visualizzata a video.
 *@return:      - void
 *
 *Descrizione	: Stampa la stringa partendo dalla posizione corrente del cursore sul LCD.
 *********************************************************************************************/
void HD44780_PutStr(char *str){
	__IO unsigned int i=0;

	do{
		HD44780_PutChar(str[i]);
		i++;
		timer_sleep(1);
	}while(str[i]!='\0');
}
/*********************************************************************************************
 *@Param:		- char: Carattere da visualizzare a video.
 *@return:      - void
 *Descrizione	: Visualizza sull LCD un singolo carattere per volta.
 *********************************************************************************************/
void HD44780_PutChar(unsigned char c){
	hd44780_wr_data(c & 0xff); /* c & 11111111 */
}
/*********************************************************************************************
 *@Param:		- char: Dato da inviare all' LCD.
 *@return:      - void
 *Descrizione	: Visualizza sull LCD il dato passato. In questo caso, trattandosi di un dato
 *				  bisogna settare il PIN RS.
 *********************************************************************************************/
void hd44780_wr_data( unsigned char data ){
	hd44780_RS_On();
	hd44780_write( data );
}
/*********************************************************************************************
 *@Param:		- char: Comando da inviare all' LCD.
 *@return:      - void
 *Descrizione	: Invia un comando all'LCD resettando il PIN RS.
 *********************************************************************************************/
void hd44780_wr_cmd( unsigned char cmd ) {
	hd44780_RS_Off();
	hd44780_write( cmd );
}
/*********************************************************************************************
 *@Param:		- char: Dato da inviare all' LCD.
 *@return:      - void
 *Descrizione	: Setta i PIN D0-D7 per l'invio di un carattere o comando.
 *********************************************************************************************/
void hd44780_write( unsigned char data ) {
	if ( data & 0x01 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT0 ));
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT0 ));
	}
	if ( data & 0x02 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT1) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT1) );
	}
	if ( data & 0x04 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT2) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT2 ));
	}
	if ( data & 0x08 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT3 ));
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT3) );
	}
	if ( data & 0x10 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4 ));
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4 ));
	}
	if ( data & 0x20 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5 ));
	}
	if ( data & 0x40 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6 ));
	}
	if ( data & 0x80 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7 ));
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7) );
	}

	/* Si abilita il PIN EN in modo da segnalare al LCD che è presenta un carattere da leggere */
	hd44780_EN_On();

	/* Attesa affinchè il carattere sia elaborato */
	hd44780_EN_high_delay();

	/* Si disabilita il PIN E */
	hd44780_EN_Off();
	hd44780_init_end_delay();
}
/*********************************************************************************************
 *@Param:		- char x: Riga del LCD.
 *@Param:		- char y: Colonna del LCD.
 *@return:      - void
 *Descrizione	: Calcola l'indirizzo a cui bisogna sposatare il cursore. Il cursore è visto
 	 	 	 	  come un carattere. Si richiama la write_cmd per spostare il cursore.
 *********************************************************************************************/
void HD44780_GotoXY(unsigned char x, unsigned char y){
	unsigned char copy_y=0;

	if(x > (HD44780_DISP_LENGTH-1))
		x = 0;

	if(y > (HD44780_DISP_ROWS-1))
		y = 0;

	switch(y)
	{
	case 0:  copy_y = 0x80; break;
	case 1:  copy_y = 0xc0; break;
	case 2:  copy_y = 0x94; break;
	case 3:  copy_y = 0xd4; break;
	}
	hd44780_wr_cmd(x + copy_y);
}
/*********************************************************************************************
 *@Param:		- void
 *@return:      - void
 *Descrizione	: Cancella il testo sul display.
 *********************************************************************************************/
void HD44780_ClrScr(void)
{
	hd44780_wr_cmd(HD44780_CMD_CLEAR);
	timer_sleep(1);
}
/*********************************************************************************************
 *@Param:		- char
 *@return:      - void
 *Descrizione	: Funzione usata solo un fase di inizializzazione per resettare il display.
 *********************************************************************************************/
void hd44780_wr_hi_nibble( unsigned char data )
{
	if ( data & 0x10 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4));
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4));
	}
	if ( data & 0x20 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5) );
	}
	if ( data & 0x40 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6) );
	}
	if ( data & 0x80 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7 ));
	}

	hd44780_EN_On();

	hd44780_EN_high_delay();

	hd44780_EN_Off();
}




