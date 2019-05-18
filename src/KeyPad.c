#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "KeyPad.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "math.h"

void keypad_init(KeyStruct* KeyPad) {
	RCC_AHBPeriphClockCmd(KEYPAD_RCC_MASKx(KEYPAD_COL_PORT_NUMBER), ENABLE);                        /* Di seguito e' riportata la funzione che abilita il bus, come ingresso   */
	RCC_AHBPeriphClockCmd(KEYPAD_RCC_MASKx(KEYPAD_ROW_PORT_NUMBER), ENABLE);						/* è presente il numero di porta, enable oppure disable                    */
	GPIO_InitTypeDef GPIO_InitStructure_COLS; 														/* Creiamo le strutture dati per la configurazione                         */
	GPIO_InitStructure_COLS.GPIO_Pin = (   	KEYPAD_PIN_MASK(KEYPAD_PIN0_COLS)	|                   /* Abilitiamo tutte le porte necessarie attraverso una OR bit a bit        */
											KEYPAD_PIN_MASK(KEYPAD_PIN1_COLS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN2_COLS)	);
	GPIO_InitStructure_COLS.GPIO_Mode = GPIO_Mode_IN;                                               /* Configuriamo il PIN in modalità Input                                   */
	GPIO_InitStructure_COLS.GPIO_OType = GPIO_OType_PP;       /* I presenti settaggi sono    */
	GPIO_InitStructure_COLS.GPIO_Speed = GPIO_Speed_50MHz;    /* necessari solo per l'output */
	GPIO_InitStructure_COLS.GPIO_PuPd = GPIO_PuPd_DOWN;                                             /* con resistore di pull down.                                             */
	GPIO_Init(KEYPAD_GPIOx(KEYPAD_COL_PORT_NUMBER), &GPIO_InitStructure_COLS);                      /* Inizzazione ottenuta passando alla funzione il numero di porta e di pin */



	GPIO_InitTypeDef GPIO_InitStructure_ROWS; 														/* Creiamo le strutture dati per la configurazione                         */
	GPIO_InitStructure_ROWS.GPIO_Pin = (   	KEYPAD_PIN_MASK(KEYPAD_PIN3_ROWS)	|                   /* Abilitiamo tutte le porte necessarie attraverso una OR bit a bit        */
											KEYPAD_PIN_MASK(KEYPAD_PIN4_ROWS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN5_ROWS)	|
											KEYPAD_PIN_MASK(KEYPAD_PIN6_ROWS)   );

	GPIO_InitStructure_ROWS.GPIO_Mode = GPIO_Mode_OUT;                             					/* Configuriamo il PIN in modalità Output                                   */
	GPIO_InitStructure_ROWS.GPIO_OType = GPIO_OType_PP;	                                            /* abilitando sia il transistore di pull UP che di pull Down                */
	GPIO_InitStructure_ROWS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_ROWS.GPIO_PuPd = GPIO_PuPd_NOPULL;											/* Essendo il PIN configurato in modalità OUTPUT non abilitiamo nessuno dei */
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	/* resistori, ne pull UP ne pull DOWN                                       */
	GPIO_Init(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER), &GPIO_InitStructure_ROWS);						/* Inizzazione ottenuta passando alla funzione il numero di porta e di pin */



	int i;																							/* Creazione del CHARSET */
	int j;

	KeyPad->index = (CODE_DIM-1);
	KeyPad->counter = 0;
	KeyPad->prevChar='\0';
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

/* @Param: 			- KeyStruct* : 	struttura con la quale è gestito in maniera atomica lo stato del tastierino.
 * @return:			- char* : 		puntatore al char premuto. Si è utilizzato un puntatore in modo da poter restituire
 * 									NULL nel caso in cui non venga premuto nessun tasto.
 *
 * Il tastierino è costituito da alcuni connettori di riga (4) e alcuni connettori di colonna (3), quando si preme un bottone
 * si chiude il circuito tra la riga r e la colonna c.
 *
 * Guardando alla configurazione precedente possiamo osservare che abbiamo configurato le righe come PIN di uscita mentre le
 * colonne sono state configurate come PIN di ingresso. Nella presente funzione abilitiamo una per volta le righe ponendo su
 * di essa un valore logico alto e su tutte le altre un valore logico basso. Sia r la riga alimentata in un certo istante se
 * i valori logici letti sui collegamenti di colonna sono tutti bassi allora non e' stato premuto nessun bottone sulla
 * presente riga r abilitata, altrimenti sia c l'indice di colonna che presenta un livello logico alto ciò significa che è
 * stato premuto il tasto in posizione (r,c).
 *
 * I PIN SONO ORDINATI DA 0 A 2. POSSIAMO OTTENERE L'INDICE DI COLONNA SEMPLICEMENTE FACENDO IL LOGARITMO DEL VALORE LETTO.
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
																							/* Logica necessaria affinche' non venga riprodotto in uscita più volte lo stesso valore         */
				KeyPad->prevChar = currChar;												/* nel caso in cui il tasto venga premuto a lungo. Per non escludere la possibilità di           */
				KeyPad->counter = 0;														/* poter inserire valori consecutivamente uguali all'interno del PIN è presente anche un counter */
				return &(KeyPad->prevChar);													/* che controlla quanto tempo è trascorso dall'ultima pressione. if(currChar != ENTER_CHAR) è    */
																							/* necessario per fare in modo da non aggioranare il prevChar se premiamo *.*/
			}

			KeyPad->counter = 0;
		}
		GPIO_ResetBits(KEYPAD_GPIOx(KEYPAD_ROW_PORT_NUMBER),KEYPAD_PIN_MASK(row_pin_number));
	}

	if(KeyPad->counter<=NO_INPUT_TIME)
		KeyPad->counter++;

	return NULL;
}

/*********************************************************************************************
 *@Param: 			- KeyStruct* : 	struttura con la quale è gestito in maniera atomica lo stato del tastierino.
 *@return:			- bool : 		restituisce true se è stata apportata una modifica al PIN memorizzato all'interno della struttura.
 *Descrizione:		- Aggiorna il codice dell'allarme.
 *********************************************************************************************/

bool get_code(KeyStruct* KeyPad){
	char* character = keypad_read(KeyPad);
	if(*character == CLEAR_CHAR){
		keypad_flush(KeyPad);
		return true;
	}

	if(KeyPad->index >= CODE_DIM) return false;
	if(character == NULL) return false;
	if(*character == ENTER_CHAR) return false;

	KeyPad->code[KeyPad->index] = *character;
	KeyPad->index++;
	return true;
}

/*********************************************************************************************
 *@Param: 			- KeyStruct* : 	struttura con la quale è gestito in maniera atomica lo stato del tastierino.
 *@return:			- void
 *
 *Descrizione: Funzione utilizzata per inizializzare la struttura KeyPad.
 *********************************************************************************************/

void keypad_flush(KeyStruct* KeyPad){
	int i;
	for(i =0; i<CODE_DIM; i++)
		KeyPad->code[i] = '_';
	KeyPad->index=0;
	KeyPad->code[CODE_DIM] = '\0';
}

