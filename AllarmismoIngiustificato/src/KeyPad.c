#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "KeyPad.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "math.h"

void keypad_init(KeyStruct* KeyPad) {
	// Enable GPIO Peripheral clock
	RCC_AHBPeriphClockCmd(KEYPAD_RCC_MASKx(KEYPAD_COL_PORT_NUMBER), ENABLE);
	RCC_AHBPeriphClockCmd(KEYPAD_RCC_MASKx(KEYPAD_ROW_PORT_NUMBER), ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure_COLS; /* Creiamo le strutture dati per la configurazione */

	/* Abilitiamo tutte le porte necessarie attraverso una OR bit a bit */
	GPIO_InitStructure_COLS.GPIO_Pin = (   	KEYPAD_PIN_MASK(KEYPAD_PIN0_COLS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN1_COLS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN2_COLS)	);

	GPIO_InitStructure_COLS.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure_COLS.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure_COLS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_COLS.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(KEYPAD_GPIOx(KEYPAD_COL_PORT_NUMBER), &GPIO_InitStructure_COLS);

	GPIO_InitTypeDef GPIO_InitStructure_ROWS; /* Creiamo le strutture dati per la configurazione */

	/* Abilitiamo tutte le porte necessarie attraverso una OR bit a bit */
	GPIO_InitStructure_ROWS.GPIO_Pin = (   	KEYPAD_PIN_MASK(KEYPAD_PIN3_ROWS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN4_ROWS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN5_ROWS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN6_ROWS)   );

	GPIO_InitStructure_ROWS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure_ROWS.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure_ROWS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_ROWS.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER), &GPIO_InitStructure_ROWS);

	int i;
	int j;

	KeyPad->index = (PIN_DIM-1);
	KeyPad->counter = 0;
	keypad_flush(KeyPad);

	char Array[] = CHARSET;
	for(i=0; i<NUM_ROWS; i++){
		for(j = 0; j<NUM_COLS; j++){
			KeyPad->keys[i][j]=Array[i*NUM_COLS + j];
		}
	}


	GPIO_ResetBits(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER),	KEYPAD_PIN_MASK(KEYPAD_PIN3_ROWS)	|
															KEYPAD_PIN_MASK(KEYPAD_PIN4_ROWS)	|
															KEYPAD_PIN_MASK(KEYPAD_PIN5_ROWS)	|
															KEYPAD_PIN_MASK(KEYPAD_PIN6_ROWS)	);

	GPIO_ResetBits(KEYPAD_GPIOx(KEYPAD_COL_PORT_NUMBER),	KEYPAD_PIN_MASK(KEYPAD_PIN0_COLS)	|
															KEYPAD_PIN_MASK(KEYPAD_PIN1_COLS)	|
															KEYPAD_PIN_MASK(KEYPAD_PIN2_COLS)	);

}

/* LOGICA:
 * ABBIAMO CONFIGURATO LE RIGHE COME PIN DI >>USCITA<<, E LE COLONNE COME PIN DI >>INGRESSO<<
 * ALIMENTIAMO LE RIGHE UNA PER VOLTA, QUINDI SAPPIAMO QUALE RIGA E' ALIMENTATA IN UN CERTO ISTANTE.
 *
 * QUANDO SI PREME UN BOTTONE SI CHIUDE IL CIRCUITO TRA LA RIGA r E LA COLONNA c.
 * IL VALORE LOGICO ALTO IN USCITA DAL CONDUTTORE DI RIGA VIENE LETTO SOLO IN CORRISPONDENZA DELLA COLONNA IN CUI E' PRESENTE IL TASTO CHE ABBIAMO PREMUTO.
 *
 * SIA ALIMENTATA LA RIGA r (NOTA) QUALE COLONNA c HA VALORE LOGICO ALTO?
 * QUINDI E' STATO PREMUTO IL TASTO (r,c) SUL TASTIERINO.
 *
 * I PIN SONO ORDINATI DA 0 A 2.
 * QUANDO SI PREME 1 SULLA COLONNA VIENE LETTO 0001 = 1
 * PER 4 0010 = 2
 * PER 7 0100 = 4
 * PER * 1000 = 8
 * POSSIAMO OTTENERE L'INDICE DI COLONNA SEMPLICEMENTE FACENDO IL LOGARITMO DEL VALORE LETTO.
 *
 * IN MANIERA PIU' EFFICIENTE SI POTEVA ITERARE SULLE COLONNE VISTO CHE SONO DI MENO.
 * MA ORA HO SONNO.
 *
 */

char* keypad_read(KeyStruct* KeyPad) {
	uint16_t dataIN;
	int row_pin_number;
	char currChar;

	for(row_pin_number=0; row_pin_number<NUM_ROWS; row_pin_number++){

		GPIO_SetBits(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER),KEYPAD_PIN_MASK(row_pin_number));
		dataIN = GPIO_ReadInputData((KEYPAD_GPIOx(KEYPAD_COL_PORT_NUMBER))) & COLS_MASK;

		if(dataIN !=0){
			uint16_t col = log2(dataIN);
			GPIO_ResetBits(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER),KEYPAD_PIN_MASK(row_pin_number));
			currChar = KeyPad->keys[row_pin_number][col];

			if(currChar != KeyPad->prevChar || KeyPad->counter>NO_INPUT_TIME){
				KeyPad->prevChar = currChar;
				KeyPad->counter = 0;
				return &(KeyPad->prevChar);
			}

			KeyPad->counter = 0;
		}
		GPIO_ResetBits(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER),KEYPAD_PIN_MASK(row_pin_number));
	}

	if(KeyPad->counter<=NO_INPUT_TIME)
		KeyPad->counter++;

	return NULL;
}

bool get_code(KeyStruct* KeyPad){
	char* character = keypad_read(KeyPad);
	if(*character == CLEAR_CHAR){
		keypad_flush(KeyPad);
		return true;
	}

	if(KeyPad->index >= PIN_DIM) return false;
	if(character == NULL) return false;
	if(character == ENTER_CHAR) return false;

	KeyPad->pin[KeyPad->index] = *character;
	KeyPad->index++;
	return true;
}

/* Funzione che re-inizializza l'array PIN e l'INDEX all'interno della struttura */
void keypad_flush(KeyStruct* KeyPad){
	int i;
	for(i =0; i<PIN_DIM; i++)
		KeyPad->pin[i] = '_';
	KeyPad->index=0;
	KeyPad->pin[PIN_DIM] = '\0';
}

