#include "rc_dfr0299.h"
#include <stdio.h>

uint8_t _received[DFPLAYER_RECEIVED_LENGTH];
uint8_t _sending[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
uint8_t _example_send_play_nor_flash[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x04, 0xFF, 0xDD, 0xEF};
uint8_t _frame_get_current_status[DFPLAYER_SEND_LENGTH] =\
        {0x7E, 0xFF, 0x06, 0x42, 0x00, 0x00, 0x00, 0xFE, 0xB9, 0xEF};

uint16_t rc_calculateCheckSum(uint8_t *buffer){
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

int rc_uart_rcv(int fserial, int debug){
    int rd=1, i;
    int offset = 0;
    int rlen = DFPLAYER_SEND_LENGTH;

    if(debug) printf("Reading response\n");
    while(rd > 0) {
        rd=read(fserial, _received + offset, rlen);
        if(rd < 0) {
            if(debug) printf("Error reading\n");
            break;
        }
        if(rd == rlen)
            break;
        offset += rd;
        rlen -= rd;
    }

    if(debug) {
        printf("debug read length %02d\n", rd);
        for(i=0; i < DFPLAYER_RECEIVED_LENGTH; i++){
            printf("0x%02X -> 0x%02X\n", i, _received[i]);
        }
    }
    if(debug) printf("Bytes received are %d \n",rd);
    return rd;
}

int rc_uart_send(int fserial, uint8_t cmd, uint8_t feedback, uint16_t data){
    int i, wr;
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
    _sending[0x07] = (uint8_t)(checksum>>8);
    _sending[0x08] = (uint8_t)(checksum&0xFF);
    _sending[0x09] = 0xEF;

    printf("DEBUG: sending frame [ ");
    for(i=0; i < 10; i++){
        printf("0x%02X,",_sending[i]);
    }
    printf(" ]\n");
    wr=write(fserial, _sending, DFPLAYER_SEND_LENGTH);
    if(wr < 0){
        printf("Uart sending error\n");
    }
    return wr;
}
