#include <stdio.h>
#include <stdint.h>

#include "bsp/bsp.h"




void imprimir_menu(uint8_t menu);

uint16_t datoRecibido;
uint8_t flagRx = 0;



int main(void) {


	typedef enum {
		inicio, estado1, estado2, estado3
	} estados_e;
	estados_e estado = inicio;

	static char string_tx[30];

	uint8_t menuImpreso = 0;
	uint16_t pasos_ADC;
	uint16_t entradaUart;
	uint8_t opcion_n2;
	float volt_ADC;
	uint8_t estado_led;
	uint8_t estado_sw;
	char onOff[4] = {' '};
	char colorLed;
	float vref = 3.3;
	uint16_t PASOSMAX = 4096;

	// Inicializo hardware

	bsp_init();

	while (1) {
		switch (estado) {
		case inicio:
			if (menuImpreso == 0) {
				imprimir_menu(0);
				menuImpreso = 1;
			}

			if (flagRx == 0)
				estado = inicio;
			else {
				estado = estado1;
				flagRx = 0;
				entradaUart = datoRecibido;
				}
				break;

				case estado1:

				if (entradaUart == '1') {

					imprimir_menu(1);
					menuImpreso = 0;

					pasos_ADC = get_ADC();

					//espero valor de uart por pulling
					while (flagRx == 0)
						;

					opcion_n2 = datoRecibido;
					flagRx = 0;

					if (opcion_n2 == '1') {

						//imprimir

						sprintf(string_tx, "\nValor ADC: %d \n\r ",pasos_ADC);
						transmit_string(string_tx);


						estado = estado1;
						break;

					}

					if (opcion_n2 == '2') {

						volt_ADC = (float) (pasos_ADC * (vref / PASOSMAX));

						//imprimir
						sprintf(string_tx, "\nValor ADC: %f V \n\r ",volt_ADC);
						transmit_string(string_tx);

						estado = estado1;
						break;
					}

					if (opcion_n2 == '3') {

						/*
						 * aca va lo de binario
						 *
						 */

						//imprimir el valor de acd en binario: asdad
						estado = estado1;
						break;
					}
					if (opcion_n2 == '0') {

						estado = inicio;
						break;

					}
				}
				estado = estado2;
				break;

				case estado2:

				if (entradaUart == '2') {

					imprimir_menu(2);
					menuImpreso = 0;

					//espero opcion
					while (flagRx == 0);

					opcion_n2 = datoRecibido;
					flagRx = 0;

					if (opcion_n2 > '0' && opcion_n2 < '5') {




						led_toggle(opcion_n2 - 49);
						estado_led = get_led(opcion_n2 - 49);

						//imprimo por uart el estado del led
						if(opcion_n2 == '1')
						colorLed = 'R';

						if(opcion_n2 == '2')
						colorLed = 'V';

						if(opcion_n2 == '3')
						colorLed = 'N';

						if(opcion_n2 == '4')
						colorLed = 'A';

						if(estado_led){

							onOff[0] = 'O';
							onOff[1] = 'n';
							onOff[2] = ' ';
						}
						else{

							onOff[0] = 'O';
							onOff[1] = 'f';
							onOff[2] = 'f';
							}


						sprintf(string_tx, "\nLed %c %s \n \r ", colorLed, onOff);
						transmit_string(string_tx);

						estado = estado2;
						break;

					}

					if (opcion_n2 == '0') {
						estado = inicio;
						break;
					}

				}

				estado = estado3;
				break;

				case estado3:

				if (entradaUart == '3') {

					imprimir_menu(3);
					menuImpreso = 0;

					while (flagRx == 0);

					opcion_n2 = datoRecibido;
					flagRx = 0;

					if (opcion_n2 > '0' && opcion_n2 < '5') {

						estado_sw = sw_getState(opcion_n2 - 49);

						//imprimo por uart el estado del led
						sprintf(string_tx, "\nSW %c: %d\n \r ", opcion_n2, estado_sw);
						transmit_string(string_tx);

						estado = estado3;
						break;
					}

					if (opcion_n2 == '0') {
						estado = inicio;
						break;
					}

				}

				estado = estado1;
				break;



		}

	}
return 0;
}

extern void APP_dato_rx(uint16_t dato) {

	datoRecibido = dato;

	flagRx = 1;

}

void imprimir_menu(uint8_t menu)
{
	char stringMenu[30];
	switch(menu)
	{
	case 0:
		sprintf(stringMenu, "\n \r1. Obtener valor ADC \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "2. Togglear Leds \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "3. Estado Switch \n \r");
		transmit_string(stringMenu);

	break;

	case 1:
		sprintf(stringMenu, "\n \r1. Obtener pasos ADC \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "2. Obtener volt ADC \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "3. Obtener binario ADC \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "0. Menu anterior \n \r");
		transmit_string(stringMenu);
	break;

	case 2:
		sprintf(stringMenu, "\n \r1. Toggle Led Rojo \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "2. Toggle Led Verde \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "3. Toggle Led Naranja  \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "4. Toggle Led Azul   \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "0. Menu anterior \n \r");
		transmit_string(stringMenu);
	break;

	case 3:
		sprintf(stringMenu, "\n \r1. SW1 \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "2. SW2 \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "3. SW3 \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "4. SW4 \n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "0. Menu anterior \n \r");
		transmit_string(stringMenu);
	}

}
