#include "dfr0299.h"
#include <stdio.h>

uint8_t _received[DFPLAYER_RECEIVED_LENGTH] =\
        {0,0,0,0,0,0,0,0,0,0};
uint8_t _sending[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
uint8_t _example_send_play_nor_flash[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x04, 0xFF, 0xDD, 0xEF};
uint8_t _frame_get_current_status[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 0x06, 0x42, 0x00, 0x00, 0x00, 0xFE, 0xB9, 0xEF};

uint16_t calculateCheckSum(uint8_t *buffer){
  uint16_t sum = 0;
  int i;
  for (i=Stack_Version; i<Stack_CheckSum; i++) {
    sum += buffer[i];
  }
  return -sum;
}

void uint16ToArray(uint16_t value, uint8_t *array){
  *array = (uint8_t)(value>>8);
  *(array+1) = (uint8_t)(value);
}

void init_uart2(void)
{    
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART2);
    /* connect port to USARTx_Tx */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* USART configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, 9600U);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_dma_receive_config(USART2, USART_DENR_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_enable(USART2);

    usart_interrupt_enable(USART2, USART_INT_RBNE);
}

int rc_uart_send(uint8_t cmd, uint8_t feedback, uint16_t data){
    int i;
    uint8_t len=6, data_msb, data_lsb;
    uint16_t checksum;

    data_msb = (uint8_t)(data>>8);
    data_lsb = (uint8_t)(data&0xFF);

    _sending[0x00] = 0x7E;
    _sending[0x01] = 0xFF;
    _sending[0x02] = len;
    _sending[0x03] = cmd;
    _sending[0x04] = feedback;
    _sending[0x05] = data_msb;
    _sending[0x06] = data_lsb;
    checksum = 0xFFFF - (0xFF + len + cmd + feedback + data_msb + data_lsb) + 1;
    _sending[0x07] = (uint8_t)((checksum>>8)&0x00FF);
    _sending[0x08] = (uint8_t)(checksum&0x00FF);
    _sending[0x09] = 0xEF;

    for(i=0; i < DFPLAYER_SEND_LENGTH; i++){
        usart_data_transmit(USART2, (uint8_t) _sending[i] );
        while ( usart_flag_get(USART2, USART_FLAG_TBE)== RESET){
            /* Waiting char is send */
        }
    }

    return i;
}

#define RCV_TIMEOUT (1000 * 1000)
int rc_uart_rcv(int debug){
    uint8_t rcvalue = 0;
    for(int j=0; j < DFPLAYER_RECEIVED_LENGTH; j++){
        int tmout = 0;
        for(tmout = 0; tmout < RCV_TIMEOUT; tmout++){
            if(RESET != usart_flag_get(USART2, USART_FLAG_RBNE))
                break;
        }
        if(tmout == RCV_TIMEOUT){
            return -1*j;
        }
        // USART2_IRQHandler
        rcvalue = usart_data_receive(USART2);
        if(j == 0 && (rcvalue != 0x7E)){
            printf("Wrong begin of frame 0x%02X\n\n", rcvalue);
            j = j - 1;
        }
        _received[j] = rcvalue;
        if(debug){
            if(j == 9){
                printf("%d:0x%02X>>\n\n", j, rcvalue);
            }else if(j >= 0){
                printf("%d:0x%02X, ", j, rcvalue);
            }
        }
    }
    if(rcvalue != 0xEF){
        printf("wrong end of frame 0x%02X\n\n", rcvalue);
    }
    return 0;
}

void rc_uart_print_rcv(void){
    int i=0;
    for(i=0; i < DFPLAYER_RECEIVED_LENGTH; i++){
        printf("0x%02X, ", _received[i]);
        if(i == DFPLAYER_RECEIVED_LENGTH - 1){
            printf("|>\n\n");
        }
    }
}