#include <stdint.h>
#include "stm32f4xx.h"			// Header del micro
#include "stm32f4xx_gpio.h"		// Perifericos de E/S
#include "stm32f4xx_rcc.h"		// Para configurar el (Reset and clock controller)
#include "stm32f4xx_syscfg.h"	// configuraciones Generales
#include "stm32f4xx_usart.h"	// libreria para modulo uart
#include "stm32f4xx_adc.h"		//libreria adc
#include "misc.h"				// Vectores de interrupciones (NVIC)
#include "bsp.h"
#include "dht22.h"

#define LED_V GPIO_Pin_12
#define LED_N GPIO_Pin_13
#define LED_R GPIO_Pin_14
#define LED_A GPIO_Pin_15

#define SW_1 GPIO_Pin_4
#define SW_2 GPIO_Pin_5
#define SW_3 GPIO_Pin_6
#define SW_4 GPIO_Pin_2

/* Puertos de los leds disponibles */
GPIO_TypeDef* leds_port[] = { GPIOD, GPIOD, GPIOD, GPIOD };
/* Leds disponibles */
uint16_t const leds[] = { LED_R, LED_V, LED_N, LED_A };

/* Puertos de los sw disponibles */
GPIO_TypeDef* sw_port[] = { GPIOE, GPIOE, GPIOE, GPIOE };
/* Leds disponibles */
uint16_t const pulsadores[] = { SW_1, SW_2, SW_3, SW_4 };

extern void APP_ISR_10ms(void);
extern void APP_dato_rx(uint8_t dato);

void led_on(uint8_t led) {
	GPIO_SetBits(leds_port[led], leds[led]);
}

void led_off(uint8_t led) {
	GPIO_ResetBits(leds_port[led], leds[led]);
}

void led_toggle(uint8_t led) {
	GPIO_ToggleBits(leds_port[led], leds[led]);
}

uint8_t get_led(uint8_t ledPin) {

	return GPIO_ReadOutputDataBit(leds_port[ledPin], leds[ledPin]);

}

uint8_t sw_getState(uint8_t sw) {

	return GPIO_ReadInputDataBit(sw_port[sw], pulsadores[sw]);
}

void leer_sensor(void) {

	DHT22_Read();
}

float leer_temperatura(void) {

	float temp;

	temp = DHT22getTemperature();

	return temp;

}

float leer_humedad(void) {

	float humedad;

	humedad = DHT22getHumidity();

	return humedad;
}

void enviar_dato(uint16_t tx_buffer) {

	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET)
		;

	USART_SendData(USART3, tx_buffer);

}

void transmit_string(char* str) {
	while (*str) {
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE))
			;
		USART_SendData(USART3, *str);
		str++;
	}
}

uint16_t get_ADC(void) {
	uint16_t valor_adc;

	// Selecciono el canal a convertir
	ADC_RegularChannelConfig(ADC1, 12, 1, ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC1);

	// Espero a que la conversi�n termine
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
		;

	// Guardo el valor leido
	valor_adc = ADC_GetConversionValue(ADC1);

	return valor_adc;
}

void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		APP_ISR_1ms();
		if (bsp_contMS) {
			bsp_contMS--;
		}

	}
}

void USART3_IRQHandler(void) {

	uint16_t data;

	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET) {

		USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		data = USART_ReceiveData(USART3);

		APP_dato_rx(data);

		//USART_SendData(USART3, data);
	}

}

//void bsp_led_init();
//void bsp_sw_init();
void bsp_uart_init();
void bsp_adc_init();
void bsp_timer3_config();
void bsp_timer2_config();


void bsp_init() {
	//bsp_led_init();
	bsp_uart_init();
	//bsp_sw_init();
	bsp_adc_init();
	bsp_timer3_config();
	bsp_timer2_config();
	DHT22_Init();

}

/**
 * @brief Inicializa Leds
 */
void bsp_led_init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Arranco el clock del periferico
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // (Push/Pull)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void bsp_sw_init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Arranco el clock del periferico
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

}

void bsp_uart_init() {

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Habilito Clocks

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// Configuro Pin TX

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

	//  Configuro Pin RX

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	//Configuro UART

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	// Inicializo la USART

	USART_Init(USART3, &USART_InitStructure);

	// Habilito la Usart

	USART_Cmd(USART3, ENABLE);

	// Habilito la Interrupcion por RX

	// USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	// Enable the USART RX Interrupt

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void bsp_adc_init() {

	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_InitTypeDef ADC1_InitStruct;

	// Habilito los clock a los perif�ricos
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Configuro el pin en modo anal�gico
	GPIO_StructInit(&GPIO_InitStruct); // Reseteo la estructura
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; // Modo Anal�gico
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Configuro el prescaler del ADC
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStruct);

	/* Configuro el ADC  */
	ADC_StructInit(&ADC1_InitStruct);
	ADC1_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_Init(ADC1, &ADC1_InitStruct);
	ADC_Cmd(ADC1, ENABLE);

}

/**
 * @brief Inicializa TIM3
 */
void bsp_timer3_config(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Habilito la interrupcion global del  TIM3 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM2 habilitado */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* Configuracion de la base de tiempo */
	TIM_TimeBaseStruct.TIM_Period = 10000; // 1 MHz bajado a 10 KHz (10 ms)
	TIM_TimeBaseStruct.TIM_Prescaler = (2 * 8000000 / 1000000) - 1; // 8 MHz bajado a 1 MHz
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
	/* TIM habilitado */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	/* TIM3 contador habilitado */
	TIM_Cmd(TIM3, ENABLE);

}

void bsp_timer2_config(void) {

	//uint16_t PrescalerValue = 0;

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;


	/* TIM2 habilitado */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_OCStructInit(&TIM_OCInitStructure);

	/* Compute the prescaler value */
	 // PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;

	/* Configuracion de la base de tiempo */
	TIM_TimeBaseStructure.TIM_Period = 65535 - 1; //en uSecs
	TIM_TimeBaseStructure.TIM_Prescaler = (2 * 8000000 / 1000000) -1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM habilitado */

	/* TIM2 contador habilitado */
	TIM_Cmd(TIM2, ENABLE);

}
