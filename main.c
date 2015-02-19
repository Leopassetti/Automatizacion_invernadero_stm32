#include <stdio.h>
#include <stdint.h>
#include "bsp/bsp.h"

#define VENT_EGESO 1
#define VENT_INGRESO 2
#define VENT_HUMEDAD 3
#define OFF 0
#define ON 1
#define LUZ_APAGADA 2000
#define TEMPMAX 25
#define TEMPMIN 16
#define HUMMIN 80
#define HUMMAX 60
#define LUZENC 60
#define LUZAPA 60

uint16_t imprimir_menu(void);

uint32_t count = 0; //contador que se incrementa cada un segundo
uint16_t datoRecibido;
uint8_t flagRx = 0;
uint8_t botonPresionado = 0;
char opcion;

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

	uint16_t seteo;

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

		if (botonPresionado == 1) {


			flagRx = 0;

			seteo = imprimir_menu();

			botonPresionado = 0;

			if(seteo != 0)
			{
				switch(opcion){

				case '2':
					tempMin = (float) seteo;
					break;
				case '3':
					tempMax = (float) seteo;
					break;
				case '4':
					humMin = (float) seteo;
					break;
				case '5' :
					humMax = (float) seteo;
					break;
				case '6' :
					luzEncendida = (uint32_t) (seteo * 3600); //pasar horas a segundos
					break;
				case '7' :
					luzApagada = (uint32_t) (seteo * 3600);
					break;
				case '8' :
					tempMin = TEMPMIN;
					tempMax = TEMPMAX;
					humMin = HUMMIN;
					humMax = HUMMAX;
					luzEncendida = LUZENC;
					luzApagada = LUZAPA;
					break;

				}



			}


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

uint16_t imprimir_menu(void) {

	char stringMenu[50];
	float temp, hum;
	uint16_t espera;
	uint16_t valorDevolver;
	uint8_t dato[4] = {0};
	uint8_t i;

	temp = leer_temperatura();
	hum = leer_humedad();

	sprintf(stringMenu, "\n \rIngrese la opcion que desea:\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "1. Humedad y temperatura actual\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "2. Setear temperatura minima \n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "3. Setear temperatura máxima\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "4. Setear humedad mínima\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "5. Setear humedad máxima\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "6. Setear tiempo de encendido de luz\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "7. Setear tiempo de apagado de luz\n \r");
	transmit_string(stringMenu);
	sprintf(stringMenu, "8. Valores por defecto \n\n \r");
	transmit_string(stringMenu);

	espera = count;
	while (flagRx == 0) {
		if (count >= (espera + 20))
			return 0;
	}

	opcion = datoRecibido;
	flagRx = 0;

	switch (opcion) {
	case '1':
		sprintf(stringMenu, "Temperatura = %.2f °C \n \r", temp);
		transmit_string(stringMenu);
		sprintf(stringMenu, "Humedad = %.2f% \n \r", hum);
		transmit_string(stringMenu);

		valorDevolver = 2;

		break;

	case '2':
		sprintf(stringMenu, "ingrese el valor de temperatura minima\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 10°C y 30°C\n \r");
		transmit_string(stringMenu);
		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;

			}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));
		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;

	case '3':
		sprintf(stringMenu, "ingrese el valor de temperatura maxima\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 20°C y 40°C\n \r");
		transmit_string(stringMenu);
		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;
		}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));
		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;

	case '4':
		sprintf(stringMenu, "ingrese el valor de humedad minima\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 20% y 60%\n \r");
		transmit_string(stringMenu);
		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;
		}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));
		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;
	case '5':
		sprintf(stringMenu, "ingrese el valor de humedad máxima\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 50% y 100%\n \r");
		transmit_string(stringMenu);
		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;
		}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));
		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;
	case '6':
		sprintf(stringMenu, "ingrese el  tiempo de encendido de luz\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 1 y 12 hs\n \r");
		transmit_string(stringMenu);
		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;
		}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));
		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;

	case '7':
		sprintf(stringMenu, "ingrese el tiempo de apagado de luz\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "valor comprendido entre 1 y 12 hs\n \r");
		transmit_string(stringMenu);

		espera = count;
		for (i = 0; i < 2; i++) {
			while (flagRx == 0) {
				if (count >= (espera + 20))
					return 0;
			}
			dato[i] = (uint8_t) datoRecibido;
			flagRx = 0;
		}

		valorDevolver = (((dato[0] - 48) * 10) + (dato[1] - 48));

		sprintf(stringMenu, "valor ingresado = %d \n \r", valorDevolver);
		transmit_string(stringMenu);

		break;

	case '8':
		sprintf(stringMenu, "Se setearan los valores por defecto\n \r");
		transmit_string(stringMenu);
		sprintf(stringMenu, "temperatura entre %d y %d\n \r", TEMPMIN, TEMPMAX);
		transmit_string(stringMenu);
		sprintf(stringMenu, "humedad entre %d y %d\n \r", HUMMIN, HUMMAX);
		transmit_string(stringMenu);
		sprintf(stringMenu, "tiempo de encendido = %d segundos\n \r", LUZENC);
		transmit_string(stringMenu);
		sprintf(stringMenu, "tiempo de apagado = %d segundos\n \r", LUZAPA);
		transmit_string(stringMenu);

		return 1;

		break;

	default:
		sprintf(stringMenu, "opcion incorrecta, presione menu nuevamente\n \r");
		transmit_string(stringMenu);
		break;
	}

	return valorDevolver;

}

