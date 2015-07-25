/*#############################################################
Driver name	    : HD44780_F3.c
Author 					: Grant Phillips
Date Modified   : 16/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description			: Provides a library to access a HD44780-based
									character LCD module.

Requirements    : * STM32F3-Discovery Board
									* Define the custom configurations on line
										80 to 112 in this file.

Functions				: HD44780_Init
									HD44780_ClrScr
									HD44780_GotoXY
									HD44780_PutStr
									HD44780_PutChar

Special Note(s) : NONE
##############################################################*/

#ifndef HD44780_F3_H
#define HD44780_F3_H
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f30x.h"
#include "Timer.h"
#include "stm32f30x_conf.h"
#include "stdbool.h"


/***************************************************************
 *
 * CONSTANTS
 *
 ***************************************************************/

/* COMMANDS */
#define HD44780_CMD_RESET            		(0x30)     	/*!< Resets display - used in init 3x */
#define HD44780_CMD_CLEAR            		(0x01)     	/*!< Clears display */
#define HD44780_CMD_RETURN_HOME      		(0x02)     	/*!< Sets DDRAM pointer to 0 */
#define HD44780_CMD_ENTRY_MODE       		(0x04)     	/*!< Sets how the pointer is updated after a character write */
#define HD44780_CMD_DISPLAY          		(0x08)     	/*!< Display settings */
#define HD44780_CMD_SHIFT            		(0x10)     	/*!< Cursor and display movement */
#define HD44780_CMD_FUNCTION         		(0x20)     	/*!< Screen type setup */
#define HD44780_CMD_CGRAM_ADDR       		(0x40)     	/*!< Sets CGRAM address */
#define HD44780_CMD_DDRAM_ADDR       		(0x80)     	/*!< Sets DDRAM address */

/* ENTRY_MODE Command parameters */
#define HD44780_ENTRY_SHIFT_DISP 			(0x01)	 	/*!< Shift display */
#define HD44780_ENTRY_SHIFT_CURS 			(0x00)	 	/*!< Shift cursor */
#define HD44780_ENTRY_ADDR_INC   			(0x02)     	/*!< Increments pointer */
#define HD44780_ENTRY_ADDR_DEC   			(0x00)	 	/*!< Decrements pointer */

/* DISPLAY Command parameters */
#define HD44780_DISP_ON       				(0x04)      /*!< Enables the display */
#define HD44780_DISP_OFF      				(0x00)      /*!< Disables the display */
#define HD44780_DISP_CURS_ON  				(0x02)      /*!< Enables cursor */
#define HD44780_DISP_CURS_OFF 				(0x00)      /*!< Disables cursor */
#define HD44780_DISP_BLINK_ON				(0x01)      /*!< Enables cursor blinking */
#define HD44780_DISP_BLINK_OFF  			(0x00)      /*!< Disables cursor blinking */

/* SHIFT Command parameters */
#define HD44780_SHIFT_DISPLAY    			(0x08)      /*!< Shifts the display or shifts the cursor if not set */
#define HD44780_SHIFT_CURSOR    			(0x00)      /*!< Shifts the display or shifts the cursor if not set */
#define HD44780_SHIFT_RIGHT      			(0x04)      /*!< Shift to the right */
#define HD44780_SHIFT_LEFT      			(0x00)      /*!< Shift to the left  */

/* FUNCTION Command parameters */
#define HD44780_FUNC_BUS_8BIT  				(0x10)      /*!< 8 bit bus */
#define HD44780_FUNC_BUS_4BIT  				(0x00)      /*!< 4 bit bus */
#define HD44780_FUNC_LINES_2   				(0x08)      /*!< 2 lines */
#define HD44780_FUNC_LINES_1   				(0x00)      /*!< 1 line */
#define HD44780_FUNC_FONT_5x10 				(0x04)      /*!< 5x10 font */
#define HD44780_FUNC_FONT_5x8  				(0x00)      /*!< 5x8 font */

/* Busy Flag - actually not used */
#define HD44780_BUSY_FLAG        			(0x80)      /*!< Busy flag */


/***************************************************************
 *
 * CONFIGURATION
 *
 ***************************************************************/

#define HD44780_CONF_BUS							HD44780_FUNC_BUS_8BIT
#define HD44780_CONF_LINES						    HD44780_FUNC_LINES_2
#define HD44780_CONF_FONT							HD44780_FUNC_FONT_5x8

#define HD44780_DISP_LENGTH						    (16)
#define HD44780_DISP_ROWS							(2)
#define HD44780_CONF_SCROLL_MS				        (20)

// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...

/* HD44780 Data lines - use the same port for all the lines */
#define HD44780_DATAPORT							(0)
#define HD44780_DATABIT0							(0)	//not used in 4-bit mode
#define HD44780_DATABIT1							(1)	//not used in 4-bit mode
#define HD44780_DATABIT2							(2)	//not used in 4-bit mode
#define HD44780_DATABIT3							(3)	//not used in 4-bit mode
#define HD44780_DATABIT4							(4)
#define HD44780_DATABIT5							(5)
#define HD44780_DATABIT6							(6)
#define HD44780_DATABIT7							(7)

/* HD44780 Control lines - use the same port for all the lines */
#define HD44780_CONTROLPORT						(1)
#define HD44780_RS_BIT							(0)
#define HD44780_RW_BIT							(1)
#define HD44780_EN_BIT							(2)

/* defines the peripheral clocks that need to be enabled for the above pins */
#define HD44780_RCC_AHBPeriph					RCC_AHBPeriph_GPIOA		//or something like "RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOD"
#define LCD_PIN_MASK(_N)                        (1 << (_N))
#define LCD_GPIOx(_N)                           ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define LCD_RCC_MASKx(_N)                       (RCC_AHBPeriph_GPIOA << (_N))

/* PWM */
#define CONTRAST_PORT_NUMBER               	(4)
#define CONTRAST_PIN_NUMBER                	(9)

#define TIMER TIM1
#define TIMER_CHANNEL2_INIT TIM_OC2Init
#define TIMER_CHANNEL1_INIT TIM_OC1Init
#define TIMER_CHANNEL3_INIT TIM_OC3Init
#define TIMER_CHANNEL4_INIT TIM_OC4Init

#define TIMER_CHANNEL2_PRELOAD_CONFIG TIM_OC2PreloadConfig
#define TIMER_CHANNEL1_PRELOAD_CONFIG TIM_OC1PreloadConfig
#define TIMER_CHANNEL3_PRELOAD_CONFIG TIM_OC3PreloadConfig
#define TIMER_CHANNEL4_PRELOAD_CONFIG TIM_OC4PreloadConfig
#define TIMER_CHANNEL_SET_COMPARE  TIM_SetCompare2
#define TIMER_CHANNEL2_SET_COMPARE TIM_SetCompare1
#define TIMER_CHANNEL3_SET_COMPARE TIM_SetCompare3
#define TIMER_CHANNEL4_SET_COMPARE TIM_SetCompare4

#define TIMER_AF GPIO_AF_2

#define TIMER_RCC_BIT RCC_APB2Periph_TIM1

#define TIMER_CLOCK_FREQUENCY 1000
#define TIMER_PERIOD 75
/***************************************************************
 *
 * FUNCTIONS
 *
 ***************************************************************/

#define hd44780_RS_On()		            GPIO_SetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_RS_BIT))
#define hd44780_RS_Off()	           	GPIO_ResetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_RS_BIT))
#define hd44780_RW_On()		            GPIO_SetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_RW_BIT))
#define hd44780_RW_Off()	            GPIO_ResetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_RW_BIT))
#define hd44780_EN_On()		            GPIO_SetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_EN_BIT))
#define hd44780_EN_Off()	            GPIO_ResetBits(LCD_GPIOx(HD44780_CONTROLPORT), LCD_PIN_MASK(HD44780_EN_BIT))

#define hd44780_EN_high_delay()			timer_sleep(3)
#define hd44780_init_delay()        	timer_sleep(16)
#define hd44780_init_delay2()       	timer_sleep(5)
#define hd44780_init_delay3()       	timer_sleep(1)
#define hd44780_init_end_delay()    	timer_sleep(2)

#define hd44780_clear()                       	  hd44780_wr_cmd( HD44780_CMD_CLEAR )
#define hd44780_home()                        	  hd44780_wr_cmd( HD44780_CMD_RETURN_HOME )
#define hd44780_entry( inc_dec, shift )           hd44780_wr_cmd( ( HD44780_CMD_ENTRY_MODE | inc_dec | shift ) & 0x07 )
#define hd44780_display( on_off, cursor, blink )  hd44780_wr_cmd( ( HD44780_CMD_DISPLAY | on_off | cursor | blink ) & 0x0F )
#define hd44780_shift( inc_dec, shift )           hd44780_wr_cmd( ( HD44780_CMD_SHIFT | inc_dec | shift ) & 0x1F )
#define hd44780_function( bus, lines, font )      hd44780_wr_cmd( ( HD44780_CMD_FUNCTION | bus | lines | font ) & 0x3F )
#define hd44780_cgram_addr( addr )                hd44780_wr_cmd( HD44780_CMD_CGRAM_ADDR | ( addr & 0x3F ) )
#define hd44780_ddram_addr( addr )                hd44780_wr_cmd( HD44780_CMD_DDRAM_ADDR | ( addr & 0x7F ) )
#define hd44780_write_char( c )                   hd44780_wr_data( c & 0xff )

void hd44780_wr_hi_nibble(unsigned char data);
void hd44780_write(unsigned char data);
void hd44780_wr_cmd(unsigned char cmd);
void hd44780_wr_data(unsigned char data);

void HD44780_Init(void);
void HD44780_PutChar(unsigned char c);
void HD44780_GotoXY(unsigned char x, unsigned char y);
void HD44780_PutStr(char *str);
void HD44780_ClrScr(void);

void pwm_init(void);
void HD44780_set_contrast(uint16_t);
void lcd_init();

#endif
