#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "gd32vf103.h"

typedef struct Noeud {
    int8_t parent; /* index du père */
    int8_t enfant; /* index de l'enfant */
    int8_t gauche; /* index du noeud gauche */
    int8_t droit;  /* index du noeud droit */
    char imgfile[13]; /* img filename TODO: utiliser un int8_t à la place */
    int16_t titre; /* numéro du mp3 titre */
    int16_t audio; /* Numéro du mp3 histoire */
} Noeud;

#define MAX_NOEUDS 100

int playlist_init(void);
int playlist_move_noeud(int noeud_dest);
int playlist_get_parent(void);
int playlist_get_enfant(void);
int playlist_get_gauche(void);
int playlist_get_droit(void);
char *playlist_get_imgfile(void);
int playlist_get_audio(void);
int playlist_get_titre(void);
void playlist_display_noeud(int noeud);

#endif /* PLAYLIST_H */