
#include "gd32vf103.h"
#include <systick.h>
#include <stdio.h>

/* Trying to read image in sd card and print on screen */
#include "string.h"
#include "dbg_led.h"
#include "volume.h"
#include "buttons.h"
#include "lcd_screen.h"
#include "ff.h"
#include "playlist.h"
#include "microsd.h"
#include "mp3player.h"

#define TICK_MS (10)

extern int noeud_actuel;

int main_init(void){
    int ret;
    longan_led_init();
    longan_led_off();
    init_uart0(); /* for printf */

    ret = lcd_screen_init();
    if(ret < 0){
        printf("lcd screen initialization error\n\n");
        return ret;
    }

    ret = microsd_init();
    if(ret < 0) {
        printf("Microsd initialization error\n\n");
        return ret;
    }

    ret = playlist_init();
    if(ret < 0) {
        printf("playlist init error\n\n");
        return ret;
    }

    /* Affichage de l'écran d'accueil */
    ret =  lcd_screen_display_from_file(playlist_get_imgfile());
    if(ret < 0){
        printf("Error printing image\n\n");
        return ret;
    }

    ret = volume_init();
    if(ret < 0){
        printf("Volume error\n\n");
        return ret;
    }

    ret = buttons_init();
    if(ret < 0){
        printf("Buttons initialization error\n\n");
        return ret;
    }

    ret = mp3player_init();
    if(ret < 0){
        printf("DFR0299 initialization error\n\n");
        return ret;
    }
    return ret;
}

int main(void)
{
    int ret;

    ret = main_init();
    if(ret < 0){
        printf("Initialization error\n\n");
        while(1){}
    }
    for(int i=1; i <= 4; i++){
        playlist_display_noeud(i);
    }


    printf("\n\n[OST] Starting with CPU=%d\n", (int)SystemCoreClock);
    printf("****************************\n\n");
    printf("* Raconte moi une histoire *\n\n");
    printf("****************************\n\n");
    delay_1ms(1000);

    while(1){
        ret = volume_mngr();
        if(ret < 0) goto VOLUME_ERR;
        ret = buttons_home_mngr();
        if(ret < 0) goto HOME_ERR;
        ret = buttons_mid_mngr();
        if(ret < 0) goto PLAYPAUSE_ERR;
        ret = buttons_prev_mngr();
        if(ret < 0) goto PREV_ERR;
        ret = buttons_next_mngr();
        if(ret < 0) goto NEXT_ERR;
        delay_1ms(TICK_MS); //TODO Manage with correct time tick interval
    }

HOME_ERR:
    printf("Home error\n\n");
    while(1){}
PREV_ERR:
    printf("Prev command error\n\n");
    while(1){}
NEXT_ERR:
    printf("next command error\n\n");
    while(1){}
PLAYPAUSE_ERR:
    printf("Play/Pause command error\n\n");
    while(1){}
VOLUME_ERR:
    printf("volume error\n\n");
    while(1){}
}
