#include <stdio.h>
#include <stdint.h>
#include "bsp/bsp.h"

#define VENT_EGESO 1
#define VENT_INGRESO 2
#define VENT_HUMEDAD 3
#define OFF 0
#define ON 1

uint32_t count = 0; //contador que se incrementa cada un segundo
uint16_t datoRecibido;
uint8_t flagRx = 0;

int main(void) {

	uint8_t tempMax = 25;

	uint8_t tempMin = 16;

	uint8_t humMax = 80;

	uint8_t humMin = 60;

	float temperatura = 0;
	float humedad = 0;
	uint16_t luz;
	uint16_t valorCuenta = 0;
	uint8_t sensorLeido = 0;

	uint16_t luzEncendida = 43200; //tiempo de la luz encendida en segundos (12 hs)
	uint16_t luzApagada = 43200;

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

		}

		if (humedad > humMax) {
			ventilador_on(VENT_EGESO);
			ventilador_on(VENT_INGRESO);
		}

		if (humedad < humMin) {
			ventilador_on(VENT_HUMEDAD);
		}

		if (luz < LUZ_APAGADA) {
			if (flagLuz == ON)
				alarma();
		}

		if (encenderLuz == 1) {

			iluminacion(ON);
			cuentaEncendido = count;
			encenderLuz = 0;

		}

		if (apagarLuz == 1) {

			iluminacion(OFF)
			cuentaApagado = count;
			apagarLuz = 0;
		}

		if (encenderLuz == 0) {
			if (count >= (cuentaEncendido + luzEncendida))
				apagarLuz = 1;
		}

		if (apagarLuz == 0) {
			if (count >= (cuentaApagado + luzApagada))
				encenderLuz = 1;
		}

	}

	return 0;
}

extern void APP_dato_rx(uint16_t dato) {

	datoRecibido = dato;

	flagRx = 1;

}

void APP_ISR_1ms(void) {

	static uint8_t count_1s = 0;

	if (count_1s++ > 100) {
		count++;
		count_1s = 0;

	}

}
