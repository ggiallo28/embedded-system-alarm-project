#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f30x.h"
#include <stdbool.h>


// ----------------------------------------------------------------------------

// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#define CHARSET                        {'1','2','3','4','5','6','7','8','9','*','0','#'}
#define NO_INPUT_TIME		     	   (50)//Conta per quanto tempo non è stato premuto nessun tasto. Per non ripetere valori.

#define KEYPAD_ROW_PORT_NUMBER         (2)
#define KEYPAD_COL_PORT_NUMBER         (3)

#define KEYPAD_PIN0_COLS 			   (0)
#define KEYPAD_PIN1_COLS 			   (1)
#define KEYPAD_PIN2_COLS 			   (2)

#define KEYPAD_PIN3_ROWS          	   (0)
#define KEYPAD_PIN4_ROWS			   (1)
#define KEYPAD_PIN5_ROWS               (2)
#define KEYPAD_PIN6_ROWS               (3)

#define NUM_ROWS                       (4)
#define NUM_COLS                       (3)

#define ROWS_MASK					   (120)
#define COLS_MASK					   (7)

#define CODE_DIM						   (4)
#define CLEAR_CHAR					 ('#')
#define ENTER_CHAR					 ('*')


typedef struct {
	char keys[NUM_ROWS][NUM_COLS];         /* Puntatore di puntatore, cioè una matrice. Mantiene il set di caratteri utilizzati.   */
	char code[CODE_DIM+1];                     /* Array PIN, serve per inserire e disinserire l'allarme                                */

	uint16_t index;                        /* Ultimo carattere valido all'interno dell'Array PIN                                   */

	char prevChar;                        /* Per evitare ripetizioni del carattere in caso di pressione a lungo del tasto          */
	uint16_t counter;

} KeyStruct;


#define KEYPAD_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define KEYPAD_PIN_MASK(_N)              (1 << (_N))
#define KEYPAD_RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))

// ----------------------------------------------------------------------------


void keypad_init(KeyStruct*);
char* keypad_read(KeyStruct*);
void keypad_flush(KeyStruct*);
bool get_code(KeyStruct*);

// ----------------------------------------------------------------------------

#endif
