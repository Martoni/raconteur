#ifndef BUTTONS_H
#define BUTTONS_H

#include "gd32vf103.h"

#define BUTTONS_GPIO_PORT GPIOB
#define BLED_GPIO_PORT GPIOA

#define BUTTONS_GPIO_CLK RCU_GPIOB
#define BLED_GPIO_CLK RCU_GPIOA

/* Bup B7 */
#define BUP_PIN GPIO_PIN_7

/* Bmid B8 */
#define BMID_PIN GPIO_PIN_8

/* Bdown B9 */
#define BDOWN_PIN GPIO_PIN_9

/* Bhome B6 A1*/
#define BHOME_PIN GPIO_PIN_6
#define LEDHOME_PIN GPIO_PIN_1

int buttons_init(void);
void buttons_display(void);

uint8_t bread_bup(void);
uint8_t bread_bmid(void);
uint8_t bread_bdown(void);
uint8_t bread_bhome(void);

void led_bhome_off();
void led_bhome_on();

int buttons_home_mngr(void);
int buttons_prev_mngr(void);
int buttons_next_mngr(void);
int buttons_mid_mngr(void);

#endif /* BUTTONS_H */