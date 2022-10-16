#include "volume.h"
#include "dfr0299.h"
#include <systick.h>
#include <stdio.h>

/* Adc0_0 max -> 3056, min 0*/
void init_ADC(){
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* Initialize the GPIO that will be used for ADC. A0-A7 and B0-B1 are connected to an ADC-channel each. */
    gpio_init(ANALOG_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, ANALOG_PIN);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);

    /* Select the clock frequency that will be used for the ADC core. Refer to README for more info on what to select. */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

    /* Reset ADC0 configuration. GD32VF103 has two internal ADCs (ADC0, ADC1). */
    adc_deinit(ADC0);

    /* Set the ADCs to work independently. Refer to the manual for the different parallel modes available. */
    adc_mode_config(ADC_MODE_FREE);

    /* Set the conversion mode to continuous. Continuous mode lets the ADC take measurements continiously without
       an external trigger. */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    /* Sets where padding is applied to the measurement. Data alignment right puts padding bits above MSB */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* Selects how many channels to convert each time. This can be used to "queue" multiple channels. Here just one channel is selected. */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);

    /* Set the channel as the first "queued" conversion each time the ADC is activated. */
    adc_regular_channel_config(ADC0, 0, ANALOG_CHANNEL, ADC_SAMPLETIME_13POINT5);

    /* Since we are using continious conversion we do not want to use an external trigger. */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* Enable ADC.*/
    adc_enable(ADC0);

    /* Let ADC stabilize */
    delay_1ms(1);

    /* Calibrates the ADC against an internal source. */
    adc_calibration_enable(ADC0);

    /* Start converting */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

int volume_init(void){
    init_ADC();
    return 0;
}

uint8_t volume_read(void){
    uint16_t analog_read;

    analog_read = ADC_RDATA(ADC0);
    return (MAX_VOLUME_DFR - (analog_read*MAX_VOLUME_DFR)/MIN_VOLUME_ADC);
}

int volume_write(uint8_t avol){
    return mp3player_set_vol(avol);
}

int volume_mngr(void){
    static uint8_t dfr_vol_old;
    uint8_t dfr_vol;
    dfr_vol = volume_read();
    if(dfr_vol <= MAX_VOLUME_DFR){
        if(dfr_vol != dfr_vol_old){
            printf("volume set %d\n", dfr_vol);
            volume_write(dfr_vol);
        }
        dfr_vol_old = dfr_vol;
    }
    return 0;
}