#ifndef VOLUME_H
#define VOLUME_H
#include "gd32vf103.h"

/* ADC test with potar */
/* From https://github.com/linusreM/Longan-RISC-V-examples/blob/master/02-ADC */
#define ANALOG_PORT              GPIOA
#define ANALOG_PIN               GPIO_PIN_0
#define ANALOG_CHANNEL           ADC_CHANNEL_0

#define MIN_VOLUME_ADC ((uint16_t)3000)
#define MAX_VOLUME_DFR ((uint16_t)25)

void init_ADC();

int volume_init(void);
uint8_t volume_read(void);
int volume_write(uint8_t avol);
int volume_mngr(void);

#endif /* VOLUME_H */