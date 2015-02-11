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


uint8_t get_led(uint8_t ledPin);

void enviar_dato(uint16_t tx_buffer);

uint8_t sw_getState(uint8_t sw);

void transmit_string(char* str);

uint16_t get_ADC(void);



void bsp_init();









#endif
