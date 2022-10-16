#include <buttons.h>
#include <stdio.h>
#include <dfr0299.h>
#include <mp3player.h>
#include <playlist.h>
#include <systick.h>

int buttons_init(void){
    /* enable the led clock */
    rcu_periph_clock_enable(BUTTONS_GPIO_CLK);
    /* configure button GPIO port */ 
    gpio_init(GPIOB, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, BUP_PIN | BMID_PIN | BDOWN_PIN | BHOME_PIN);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDHOME_PIN);

    return 0;
}

void buttons_display(void){
    printf("bup   %d\n", bread_bup());
    printf("bmid  %d\n", bread_bmid());
    printf("bdown %d\n", bread_bdown());
    printf("bhome %d\n", bread_bhome());
}

/* Bup B7 */
uint8_t bread_bup(void){
    return gpio_input_bit_get(BUTTONS_GPIO_PORT, BUP_PIN);
}

/* Bmid B8 */
uint8_t bread_bmid(void){
    return gpio_input_bit_get(BUTTONS_GPIO_PORT, BMID_PIN);
}

/* Bdown B9 */
uint8_t bread_bdown(void){
    return gpio_input_bit_get(BUTTONS_GPIO_PORT, BDOWN_PIN);
}

/* Bhome B6 A1*/
uint8_t bread_bhome(void){
    return gpio_input_bit_get(BUTTONS_GPIO_PORT, BHOME_PIN);
}

void led_bhome_on()
{
    GPIO_BOP(BLED_GPIO_PORT) = LEDHOME_PIN;
}

void led_bhome_off()
{
    GPIO_BC(BLED_GPIO_PORT) = LEDHOME_PIN;
}

int buttons_home_mngr(void){
    static uint8_t button_home_old = 0;
    static uint8_t led_on = 0;
    int ret = 0;

    uint8_t button_home = bread_bhome();
    if((button_home == 1) && (button_home_old == 0)){
        led_on = !led_on;
        if(led_on){
            led_bhome_off();
        }else{
            led_bhome_on();
        }
        int parent = playlist_get_parent();
        if(parent >= 0){
            printf("debug parent %d\n\n", parent);
            ret = playlist_move_noeud(parent);
            if(ret < 0){
                printf("move noeud erreur\n\n");
                return -1;
            }
        }
    }
    button_home_old = button_home;
    return 0;
}

int buttons_mid_mngr(void){
    static uint8_t playing = 0;
    static uint8_t pause = 1;
    static int track = -1;
    static uint8_t button_mid_old = 0;
    int ret = 0;
    uint8_t button_mid = bread_bmid();
    if((button_mid == 1) && (button_mid_old == 0)){
        /* si le noeud possède un enfant, on y va */
        int child = playlist_get_enfant();
        if(child >= 0){
            ret = playlist_move_noeud(child);
            if(ret < 0){
                printf("move enfant erreur\n\n");
                return -1;
            }
            playing = 0;
        } else { /* sinon on joue l'histoire */
            printf("debug mid push (playing %d)\n\n", playing);
            if(playing == 0){ /* lancement de l'histoire */
                track = playlist_get_audio();
                printf("debug playing node audio %d\n\n", track);
                if(track < 0){
                    printf("mauvais audio %d\n\n", track);
                    return -1;
                }
                ret = mp3player_set_track(track);
                if(ret < 0){
                    printf("Can't set track %d\n\n", ret);
                    return -1;
                }
                delay_1ms(100);
                ret = mp3player_play();
                playing = 1;
            } else { /* play/pause si déjà lancée */
                pause = !pause;
                if(pause){
                    ret = mp3player_play();
                }else{
                    ret = mp3player_pause();
                }
            }
        }
    }
    button_mid_old = button_mid;
    return ret;
}

int buttons_prev_mngr(void){
    static uint8_t button_prev_old = 0;
    int ret = 0;


    uint8_t button_prev = bread_bdown();
    if((button_prev_old == 0) && (1 == button_prev)){
        int gauche = playlist_get_gauche();
        if(gauche >= 0){
            printf("debug gauche move to %d\n\n", gauche);
            ret = playlist_move_noeud(gauche);
            if(ret < 0){
                printf("move noeud prev erreur\n\n");
                return -1;
            }
        }
    }
    button_prev_old = button_prev;
    return ret;
}

int buttons_next_mngr(void){
    static uint8_t button_next_old = 0;
    int ret = 0;

    uint8_t button_next = bread_bup();
    if((button_next_old == 0) && (1 == button_next)){
        int droit = playlist_get_droit();
        if(droit >= 0){
            printf("debug move to droit %d\n\n", droit);
            ret = playlist_move_noeud(droit);
            return ret;
        }
    }
    button_next_old = button_next;
    return ret;
}