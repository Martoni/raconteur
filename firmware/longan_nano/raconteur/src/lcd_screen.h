#ifndef LCD_SCREEN_H
#define LCD_SCREEN_H

#include <stdio.h>

#define LCD_WIDTH 160
#define LCD_HEIGH 80

int lcd_screen_init(void);
uint16_t *lcd_get_image_buffer(void);
int lcd_screen_display_image(void);
int lcd_screen_display_from_file(char * filename);

#endif /* LCD_SCREEN_H */