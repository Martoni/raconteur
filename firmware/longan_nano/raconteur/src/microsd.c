#include "microsd.h"
#include "gd32vf103.h"
#include "gd32v_tf_card_if.h"
#include "lcd_screen.h"

static FATFS fs;
static FIL fil;       

int microsd_read_image_file(char *filename){
    FRESULT fr;
    UINT br = 0;
    uint16_t *image_buffer;
    image_buffer = lcd_get_image_buffer();

    fr = f_open(&fil, filename, FA_READ);
    if(fr != 0) {
        printf("Can't open %s error %d\n\n", filename, fr);
        return -1;
    }
    fr = f_read(&fil, image_buffer, LCD_WIDTH*LCD_HEIGH*2, &br);
    if(fr != 0){
        printf("Can't read image error %d\nn", fr);
        return -1;
    }
    fr = f_close(&fil);
    if(fr != 0){
        printf("Error closing file %s\n\n", filename);
        return -1;
    }
    return br;
}

int microsd_open_file(char *filename){
    FRESULT fr;

    fr = f_open(&fil, filename, FA_READ);
    if(fr != 0) {
        printf("Can't open %s error %d\n\n", filename, fr);
        return -1;
    }
    return 0;
}

int microsd_read1byte_file(void){
    FRESULT fr;
    UINT br = 0;

    uint8_t bytevalue = 0;
    fr = f_read(&fil, &bytevalue, 1, &br);
    if(fr != 0) {
        printf("Error reading 1 byte in file\n\n");
        return -1;
    }
    return bytevalue;
}


int microsd_close_file(void){
    FRESULT fr;

    fr = f_close(&fil);
    if(fr != 0) {
        printf("Can't close file error %d\n\n", fr);
        return -1;
    }
    return 0;
}

int microsd_init(void){
    volatile FRESULT fr;

    /* Sets a valid date for when writing to file */
    set_fattime(1980, 1, 1, 0, 0, 0);

    /* This function "mounts" the SD-card which makes the filesystem available */
    fr = f_mount(&fs, "", 1);       //Mount storage device
    if(fr < 0){
        printf("Mount error\n\n");
        return -1;
    }

    return 0;
}