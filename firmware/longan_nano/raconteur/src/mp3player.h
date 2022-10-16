#ifndef MP3PLAYER_H
#define MP3PLAYER_H

int mp3player_init(void);

int mp3player_get_status(void);

int mp3player_set_vol(uint16_t avol);
int mp3player_next_track(void);
int mp3player_prev_track(void);
int mp3player_set_track(uint16_t atrack);
int mp3player_play(void);
int mp3player_pause(void);

#endif /* MP3PLAYER_H */