#include <stdio.h>
#include <mp3player.h>
#include <systick.h>
#include "dfr0299.h"

int mp3player_init(void){
    int ret = 0;

    init_uart2(); /* for dfr */

    ret = rc_uart_send(CMD_RESET, 0, 0);
    if(ret < 0){
        printf("Can't send reset command to dfr0299\n\n");
        return ret;
    }

    printf("Waiting 3 sec\n\n");
    delay_1ms(3000);

    printf("getting mp3player status\n\n");
    ret = mp3player_get_status();
    printf("returned status %d\n\n", ret);
    ret = mp3player_set_vol(4);
    printf("get status \n");
 
    printf("Tfcard\n\n");
    ret = rc_uart_send(CMD_TFCARDFILESNUM, 0, 0);
    ret = rc_uart_rcv(1);
    if(ret < 0){
        printf("Can't read tfcardfilenum\n\n");
    }

    return ret;
}

int mp3player_get_status(void){

    int ret = 0;
    ret = rc_uart_send(CMD_CURRENTSTATUS, 0, 0);
    if(ret < 0){
        printf("Can't send status %d\n\n", ret);
    }
    /*delay_1ms(1);*/
    ret = rc_uart_rcv(1);
    if(ret < 0){
        printf("Timeout %d\n\n", ret);
    }
    
    printf("got mp3player status \n\n");
    return ret;
}

int mp3player_set_vol(uint16_t avol){
    int ret;
    ret = rc_uart_send(CMD_SPECVOL, 0, avol);
    if(ret < 0){
        printf("Can't specify volume %d\n\n", avol);
    }
    return ret;
}

int mp3player_next_track(void){
    return rc_uart_send(CMD_NEXT, 0, 0);
}

int mp3player_prev_track(void){
    return rc_uart_send(CMD_PREVIOUS, 0, 0);
}

int mp3player_set_track(uint16_t atrack){
    return rc_uart_send(CMD_SPECTRACK, 0, atrack);
}

int mp3player_play(void){
    int ret;
    ret = rc_uart_send(CMD_PLAYBACK, 0, 0);
    return ret;
}

int mp3player_pause(void){
    int ret;
    ret = rc_uart_send(CMD_PAUSE, 0, 0);
    return ret;
}
