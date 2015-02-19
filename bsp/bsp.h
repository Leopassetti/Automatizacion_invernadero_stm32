#ifndef _BSP_H
#define _BSP_H

/**
 * @brief Prende un led
 *
 * @param led Led a prender
 */
void led_on(uint8_t led);

/**
 * @brief Apaga un led
 *
 * @param led Led a apagar
 */
void led_off(uint8_t led);

/**
 * @brief Delay por software
 */

void led_toggle(uint8_t led);
/**
 * @brief conmutar un led
 *
 * @param led Led a conmutar
 *
 */

void leer_sensor(void);

float leer_temperatura(void);

float leer_humedad(void);




void enviar_dato(uint16_t tx_buffer);


void transmit_string(char* str);

void ventilador_on(uint8_t vent_n);

void ventilador_off(uint8_t vent_n);

void iluminacion(uint8_t estado);

void bsp_delay10Ms(uint16_t x);

float leer_temperatura_lm335(void);


uint16_t get_luz(void);



void bsp_init();









#endif
