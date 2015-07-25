//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "stm32f30x_tim.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "Buzzer.h"


// ----------------------------------------------------------------------------

void buzzer_init() {

	//////////////// SET PIN ///////////////////////

	// Enable GPIO Peripheral clock

	// è la funzione che abilita il bus, come ingresso vuole la porta, enable o disable, però noi dabbiamo utilizzare la maschera per la selezione della porta (per comodità, omettiamo direttamente la porta)
	RCC_AHBPeriphClockCmd(BUZZER_RCC_MASKx(BUZZER_PORT_NUMBER), ENABLE);
	// definiamo una struttura
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure pin in output push/pull mode
	// riempiamo la struttura creata prima
	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN_MASK(BUZZER_PIN_NUMBER); //mettiamo il pin, abbiamo utilizzato sempre la famosa maschera
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //inseriamo la modalità AF --> Alternate Function
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // mettiamo la configurazione di uscita: Push Pull o Open Drain. Scegliamo la configurazione PP poichè per pilotare il buzzer non abbiamo bisogno nè di una tensione nè di una corrente alta
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // è quella massima
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // non lo configuriamo perchè serve solo per l'ingresso mentre il buzzer è un'uscita, pull up o pull down
	GPIO_Init(BUZZER_GPIOx(BUZZER_PORT_NUMBER), &GPIO_InitStructure); //inizializziamo il pin, gli passiamo numero della porta e il pin
	GPIO_PinAFConfig(BUZZER_GPIOx(BUZZER_PORT_NUMBER), BUZZER_PIN_NUMBER, BUZZER_TIMER_AF); // modalità di configurazione del tipo di AF
	//////////////// FINE SET PIN //////////////////////

	///////////////// SET TIMER //////////////////////

	// Enable Timer clock
	RCC_APB1PeriphClockCmd(BUZZER_TIMER_RCC_BIT, ENABLE); // configuriamo la funzione che abilita il clock del bus, gli passiamo il bus desiderato e enable

	RCC_ClocksTypeDef RCC_Clocks; // mi serve la variabile RCC_Clocks per il clock di sistema (è un numero)
	RCC_GetClocksFreq(&RCC_Clocks); // vado a prendere il clock dichiarato sopra

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // inizilizziamo la struttura del timer

	uint32_t prescaler = RCC_Clocks.PCLK2_Frequency / BUZZER_TIMER_CLOCK_FREQUENCY - 1; // vedo quanto vale il prescaler, faccio una divisione, cioè la freq del processore diviso la freq con la quale conta il timer (timer clock frequency) C'è il -1 per esigenze di configurazione
	uint32_t period = BUZZER_TIMER_CLOCK_FREQUENCY / 262; //  metto un valore di configurazione iniziale (posso mettere qualsiasi valore)
	TIM_TimeBaseStructure.TIM_Period = period - 1; // è un fatto di configurazione, period (uguale ad N) è il valore massimo del timer prima dell'azzeramento, perciò se voglio che il timer conti fino a 100 (0 a 99) dobbiamo mettere 100 - 1
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler; // diamo il prescaler
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // è un'ulteriore divisione del clock, in questo caso è 1 che significa non dividere!
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // modalità di conteggio verso alto, cioè da 0 fino a period
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //ogni quanto ripeto a contare, in questo caso è 0 per sempre, inizio sempre da capo
	TIM_TimeBaseInit(BUZZER_TIMER, &TIM_TimeBaseStructure); //inizializzo il timer
	///////////////// FINE SET TIMER /////////////////////

	//////////////// SET CANALE /////////////////////

	// Configure timer channel
	TIM_OCInitTypeDef TIM_OCInitStructure; // definiamo una struttura

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //scelgo la modalità del PWM, i canali di output supportano più PWM,in questo caso scegliamo la modalità 1 che è quella che conosciamo.
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // indica se vogliamo o meno abilitare il passaggio dell'output verso il pin del processore. (lo abilitiamo)
	TIM_OCInitStructure.TIM_Pulse = 0; // lo lasciamo al valore di default poichè non ci interessa
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // ci consente di scegliere se vogliamo che il segnale di output sia alto in corrispondenza del valore logico alto del PWM o invertito. Noi scegliamo la prima opzione
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	// These settings must be applied on the timer 1.
	// le prossime 3 istruzioni non ci interessano, infatti disabilitiamo il segnale complementare che si trova su un altro PIN
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;

	BUZZER_TIMER_CHANNEL_INIT(BUZZER_TIMER, &TIM_OCInitStructure); // funzione che inizializza il canale di output, è solo un alias che abbiamo definito nel .h
	BUZZER_TIMER_CHANNEL_PRELOAD_CONFIG(BUZZER_TIMER, TIM_OCPreload_Enable); // chiamata a funzione che abilita il registro di precaricamento del canale, anche questo è un alias definito nel .h

	///////////// FINE SET CANALE /////////////////

	//TIM_CtrlPWMOutputs(TIMER, ENABLE);
	TIM_Cmd(BUZZER_TIMER, ENABLE); // abilitiamo il timer che fino ad ora era solo configurato.
}

// ----------------------------------------------------------------------------


