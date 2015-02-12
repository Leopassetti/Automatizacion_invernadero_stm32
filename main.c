#include <stdio.h>
#include <stdint.h>
#include "bsp/bsp.h"

#define VENT_EGESO 1
#define VENT_INGRESO 2
#define VENT_HUMEDAD 3
#define OFF 0
#define ON 1
#define LUZ_APAGADA 2000

void imprimir_menu(void);

uint32_t count = 0; //contador que se incrementa cada un segundo
uint16_t datoRecibido;
uint8_t flagRx = 0;
uint8_t botonPresionado = 0;

int main(void) {

	float tempMax = 25;

	float tempMin = 16;

	float humMax = 80;

	float humMin = 60;

	float temperatura = 0;
	float humedad = 0;
	uint16_t luz;
	uint16_t valorCuenta = 0;
	uint8_t sensorLeido = 0;

	uint32_t luzEncendida = 60; //tiempo de la luz encendida en segundos (12 hs) 43200
	uint32_t luzApagada = 60;

	uint32_t cuentaApagado = 0;
	uint32_t cuentaEncendido = 0;

	uint8_t apagarLuz = 0;
	uint8_t encenderLuz = 1;

	uint8_t flagE = 0;
	uint8_t flagA = 0;

//inicializo hardware

	bsp_init();

	while (1) {

		if (sensorLeido == 0) {

			valorCuenta = count;
			leer_sensor();
			sensorLeido = 1;
		}

		if (count >= (valorCuenta + 2))
			sensorLeido = 0;

		luz = get_luz();

		temperatura = leer_temperatura();

		humedad = leer_humedad();

		if (temperatura >= tempMin && temperatura <= tempMax) {
			if (humedad >= humMin && humedad <= humMax) {
				ventilador_off(VENT_EGESO);
				ventilador_off(VENT_INGRESO);
				ventilador_off(VENT_HUMEDAD);

			}
		}

		if (temperatura > tempMax) {
			ventilador_on(VENT_EGESO);
			ventilador_on(VENT_INGRESO);

		}

		if (temperatura < tempMin) {
			//ENCENDER LED PARA AVISAR
			led_on(1);

		}

		if (humedad > humMax) {
			ventilador_on(VENT_EGESO);
			ventilador_on(VENT_INGRESO);
		}

		if (humedad < humMin) {
			ventilador_on(VENT_HUMEDAD);
		}

		if (encenderLuz == 1) {

			iluminacion(ON);

			if (flagE == 0) {
				cuentaEncendido = count;
				flagE = 1;
			}

			if (count >= (cuentaEncendido + luzEncendida)) {
				apagarLuz = 1;
				encenderLuz = 0;
				flagE = 0;
			}

		}

		if (apagarLuz == 1) {

			iluminacion(OFF);

			if (flagA == 0) {
				cuentaApagado = count;
				flagA = 1;
			}

			if (count >= (cuentaApagado + luzApagada)) {
				apagarLuz = 0;
				encenderLuz = 1;
				flagA = 0;
			}

		}


		if (luz < LUZ_APAGADA) {
			if (encenderLuz == 1)
								//alarma();
				led_on(2);
			else
				led_off(2);
		}

		if(botonPresionado == 1){

			botonPresionado = 0;
			imprimir_menu();

		}

	}

	return 0;
}

extern void APP_dato_rx(uint16_t dato) {

	datoRecibido = dato;

	flagRx = 1;

}

void APP_ISR_10ms(void) {

	static uint8_t count_1s = 0;

	if (count_1s++ > 100) {
		count++;
		count_1s = 0;

	}

}

void APP_ISR_sw(void) {

	botonPresionado = 1;
}

void imprimir_menu(void){

	char stringMenu[30];
		float temp, hum;
		char opcion;

		temp = leer_temperatura();
		hum = leer_humedad();

		sprintf(stringMenu, "\n \rEste va a ser el menu\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "1. humedad \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "2. temperatura \n \r");
		transmit_string(stringMenu);


		sprintf(stringMenu, "2. temperatura = %f \n \r", temp);
		transmit_string(stringMenu);



		sprintf(stringMenu, "1. humedad = %f \n \r", hum);
		transmit_string(stringMenu);



}


