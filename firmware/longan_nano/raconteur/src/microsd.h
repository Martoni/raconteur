#ifndef MICRO_SD_H
#define MICRO_SD_H

int microsd_init(void);
int microsd_read_image_file(char *filename);
int microsd_open_file(char *filename);
int microsd_read1byte_file(void);
int microsd_close_file(void);

#endif  /* MICRO_SD_H */