#include "lcd_screen.h"
#include "lcd.h"
#include "dbg_led.h"
#include "gd32vf103.h"
#include "string.h"
#include <playlist.h>
#include <microsd.h>

/* A buffer for storing an image from the SD-card */
static uint16_t image_buffer[160*80] = {0};

int lcd_screen_init(void){
    Lcd_Init();
    LCD_Clear(BLACK);
    return 0;
}

uint16_t *lcd_get_image_buffer(void){
    return image_buffer;
}

int lcd_screen_display_image(void){
    LCD_ShowPicture(0, 0, LCD_WIDTH-1, LCD_HEIGH-1, (u8*)image_buffer);
    return 0;
}

int lcd_screen_display_from_file(char * filename){
    int ret=0;
    ret = microsd_open_file(filename);
    if(ret < 0){
        printf("Can't open image file\n\n");
        return -1;
    }

	int i;
	int size = LCD_WIDTH * LCD_HEIGH * 2;
	LCD_Address_Set(0, 0, LCD_WIDTH-1, LCD_HEIGH-1);
	for(i=0;i<size;i++) { 	
        int pixel = microsd_read1byte_file();
        if(pixel < 0) {
            printf("Error reading byte in file\n\n");
            return -1;
        }
		LCD_WR_DATA8((u8)pixel);
	}	

    ret = microsd_close_file();
    if(ret < 0){
        printf("Can't close image file\n\n");
        return -1;
    }
    return 0;
}