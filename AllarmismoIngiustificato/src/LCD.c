#include "LCD.h"
#include "stm32f30x_tim.h"

void lcd_init(){
	HD44780_Init();
	pwm_init();
	HD44780_set_contrast(20);
}

/*********************************************************************************************
Function name   : HD44780_Init
Author 					: Grant Phillips
Date Modified   : 16/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Initializes the HD44780 lcd module

Special Note(s) : NONE

Parameters			: NONE
Return value		: NONE
 *********************************************************************************************/

void HD44780_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;											//structure used for setting up a GPIO port
	RCC_ClocksTypeDef 			RCC_Clocks;															//structure used for setting up the SysTick Interrupt

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);


	//GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure the peripheral clocks for the HD44780 data and control lines */
	RCC_AHBPeriphClockCmd(LCD_RCC_MASKx(HD44780_DATAPORT), ENABLE);

	/* Configure the HD44780 Data lines (DB7 - DB4) as outputs*/
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

	/* Configure the HD44780 Control lines (RS, RW, EN) as outputs*/
	GPIO_InitStructure.GPIO_Pin = (	LCD_PIN_MASK(HD44780_EN_BIT) |
			LCD_PIN_MASK(HD44780_RW_BIT) |
			LCD_PIN_MASK(HD44780_RS_BIT) );

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_GPIOx(HD44780_CONTROLPORT), &GPIO_InitStructure);

	/* clear control bits */
	hd44780_EN_Off();
	hd44780_RS_Off();
	hd44780_RW_Off();

	/* wait initial delay for LCD to settle */
	/* reset procedure - 3 function calls resets the device */
	hd44780_init_delay();
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );
	hd44780_init_delay2();
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );
	hd44780_init_delay3();
	hd44780_wr_hi_nibble( HD44780_CMD_RESET );

#if HD44780_CONF_BUS == HD44780_FUNC_BUS_4BIT
	/* 4bit interface */
	hd44780_wr_hi_nibble( HD44780_CMD_FUNCTION );
#endif /* HD44780_CONF_BUS == HD44780_FUNC_BUS_4BIT */

	/* sets the configured values - can be set again only after reset */
	hd44780_function( HD44780_CONF_BUS, HD44780_CONF_LINES, HD44780_CONF_FONT );

	/* turn the display on with no cursor or blinking */
	hd44780_display( HD44780_DISP_ON, HD44780_DISP_CURS_OFF, HD44780_DISP_BLINK_OFF );

	/* clear the display */
	hd44780_clear();

	/* addr increment, shift cursor */
	hd44780_entry( HD44780_ENTRY_ADDR_INC, HD44780_ENTRY_SHIFT_CURS );


}

void pwm_init(){
	RCC_AHBPeriphClockCmd(LCD_RCC_MASKx(CONTRAST_PORT_NUMBER), ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure pin in output push/pull mode
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_MASK(CONTRAST_PIN_NUMBER);
	GPIO_Init(LCD_GPIOx(CONTRAST_PORT_NUMBER), &GPIO_InitStructure);
	GPIO_PinAFConfig(LCD_GPIOx(CONTRAST_PORT_NUMBER), CONTRAST_PIN_NUMBER, TIMER_AF);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_ClocksTypeDef RCC_Clocks;
	// Configure timer channel
	TIM_OCInitTypeDef TIM_OCInitStructure;
	uint32_t prescaler=0;

	// Enable Timer clock

	RCC_APB2PeriphClockCmd(TIMER_RCC_BIT, ENABLE);

	// Configure timer

	RCC_GetClocksFreq(&RCC_Clocks);
	prescaler=RCC_Clocks.PCLK2_Frequency/TIMER_CLOCK_FREQUENCY-1;
	TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMER, &TIM_TimeBaseStructure);



	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// These settings must be applied on the timer 1.
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;

	TIMER_CHANNEL1_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL1_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL2_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL2_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL3_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL3_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	TIMER_CHANNEL4_INIT(TIMER, &TIM_OCInitStructure);
	TIMER_CHANNEL4_PRELOAD_CONFIG(TIMER, TIM_OCPreload_Enable);

	// Enable timer
	HD44780_set_contrast(0);

	TIM_CtrlPWMOutputs(TIMER, ENABLE);
	TIM_Cmd(TIMER, ENABLE);
}

/* Function used from the CooCox HD44780 library */
/********************************************************************************************/
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

	/* set the EN signal */
	hd44780_EN_On();

	/* wait */
	hd44780_EN_high_delay();

	/* reset the EN signal */
	hd44780_EN_Off();
}


#if HD44780_CONF_BUS == HD44780_FUNC_BUS_4BIT

void hd44780_wr_lo_nibble( unsigned char data )
{
	if ( data & 0x01 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT4) );
	}
	if ( data & 0x02 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT5 ));
	}
	if ( data & 0x04 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT6) );
	}
	if ( data & 0x08 ) {
		GPIO_SetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7) );
	} else {
		GPIO_ResetBits( LCD_GPIOx(HD44780_DATAPORT), LCD_PIN_MASK(HD44780_DATABIT7) );
	}

	/* set the EN signal */
	hd44780_EN_On();

	/* wait */
	hd44780_EN_high_delay();

	/* reset the EN signal */
	hd44780_EN_Off();
}

/* 4bit bus version */
void hd44780_write( unsigned char data )
{
	/* send the data bits - high nibble first */
	hd44780_wr_hi_nibble( data );
	hd44780_wr_lo_nibble( data );
}
#endif /* HD44780_CONF_BUS == HD44780_FUNC_BUS_4BIT */


#if HD44780_CONF_BUS == HD44780_FUNC_BUS_8BIT

/* 8bit bus version */
void hd44780_write( unsigned char data )
{
	/* set the data bits */
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

	/* tell the lcd that we have a command to read in */
	hd44780_EN_On();

	/* wait long enough so that the lcd can see the command */
	hd44780_EN_high_delay();

	/* reset the ce line */
	hd44780_EN_Off();
	hd44780_init_end_delay();

}
#endif /* HD44780_CONF_BUS == HD44780_FUNC_BUS_8BIT */


void hd44780_wr_cmd( unsigned char cmd )
{
	hd44780_RS_Off();
	hd44780_write( cmd );
}


void hd44780_wr_data( unsigned char data )
{
	hd44780_RS_On();
	hd44780_write( data );
}
/********************************************************************************************/

/*********************************************************************************************
Description			: Creates a delay which is simply a code loop which is independent from any
					  hardware timers.

Special Note(s) : NONE

Parameters			: del		-	32-bit value to represent the delay cycles
Return value		: NONE
 *********************************************************************************************/
void hd44780_Delay(unsigned long del)
{
	unsigned long i=0;

	while(i<del)
		i++;
}

void
HD44780_set_contrast(uint16_t value) {
	TIMER_CHANNEL_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL2_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL3_SET_COMPARE(TIMER, value);
	TIMER_CHANNEL4_SET_COMPARE(TIMER, value);
}

/*********************************************************************************************
Function name   : HD44780_PutChar
Author 					: Grant Phillips
Date Modified   : 16/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Prints a character at the current character cursor position

Special Note(s) : NONE

Parameters			: c				-	character to print

Return value		: NONE
 *********************************************************************************************/
void HD44780_PutChar(unsigned char c)
{
	hd44780_wr_data(c & 0xff);
}

/*********************************************************************************************
Description			: Changes the position of the current character cursor from where the next 
					  characters will be printed.

Parameters			: x	-	column position (0 - HD44780_DISP_LENGTH-1)
					  y	-	row position (0 - HD44780_DISP_ROWS-1)

Return value		: NONE
 *********************************************************************************************/
void HD44780_GotoXY(unsigned char x, unsigned char y)
{
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
Function name   : HD44780_PutStr
Author 					: Grant Phillips
Date Modified   : 16/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Prints a string at the current character cursor position.

Special Note(s) : NONE

Parameters			: str			-	string (char array) to print

Return value		: NONE
 *********************************************************************************************/
void HD44780_PutStr(char *str)
{
	__IO unsigned int i=0;

	do
	{
		HD44780_PutChar(str[i]);
		i++;
		timer_sleep(1);
	}while(str[i]!='\0');
}



/*********************************************************************************************
Function name   : HD44780_ClrScr
Author 					: Grant Phillips
Date Modified   : 16/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Clears the display

Special Note(s) : NONE

Parameters			: NONE
Return value		: NONE
 *********************************************************************************************/
void HD44780_ClrScr(void)
{
	hd44780_wr_cmd(HD44780_CMD_CLEAR);
	timer_sleep(1);
}

