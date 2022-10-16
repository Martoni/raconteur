#ifndef DBG_LED
#define DBG_LED

#include "gd32vf103.h"

/* BUILTIN LED OF LONGAN BOARDS IS PIN PC13 */
#define LED_PIN GPIO_PIN_13
#define LED_GPIO_PORT GPIOC
#define LED_GPIO_CLK RCU_GPIOC

/* green led */
//#define LED_PIN GPIO_PIN_1
//#define LED_GPIO_PORT GPIOA
//#define LED_GPIO_CLK RCU_GPIOA

void longan_led_init();
void longan_led_on();
void longan_led_off();

int _put_char(int ch);
void init_uart0(void);

#endif /* DBG_LED */