#include "playlist.h"
#include <stdio.h>
#include <lcd_screen.h>
#include <mp3player.h>

static struct Noeud *arbre[MAX_NOEUDS] = {NULL};

/* 1 */
static struct Noeud accueil = {
    .parent = -1,
    .enfant = 2,
    .gauche = -1,
    .droit = -1,
    .imgfile = "accueil.bin",
    .titre = -1,
    .audio = -1
};

/* 2 */
static struct Noeud belhist = {
    .parent = 1,
    .enfant = 3,
    .gauche = 2,
    .droit = 2,
    .imgfile = "belhist.bin",
    .titre = 1,
    .audio = -1
};

/* 3 */
static struct Noeud colsven = {
    .parent = 2,
    .enfant = -1,
    .gauche = 4,
    .droit = 4,
    .imgfile = "colsven.bin",
    .titre = 5,
    .audio = 4
};

/* 4 */
static struct Noeud voyext = {
    .parent = 2,
    .enfant = -1,
    .gauche = 3,
    .droit = 3,
    .imgfile = "voyext.bin",
    .titre = 3,
    .audio = 2
};

static int8_t noeud_actuel;

/* < 0 : error
   0 : no move
   1 : moved */

int playlist_move_noeud(int noeud_dest){
    int ret;
    printf("debug %d -> %d\n\n", noeud_actuel, noeud_dest);
    if(noeud_dest < 0){
        return -1;
    }
    if(noeud_dest >= MAX_NOEUDS){
        printf("Noeud de destination trop grand\n\n");
        return -1;
    }
    if(arbre[noeud_dest] == NULL){
        printf("Pas de noeud en %d\n\n", noeud_dest);
        return -1;
    }
    /* Si on a changé de nœud, on joue le titre 
        et on change l'image 
    */
    if(noeud_actuel != noeud_dest){
        noeud_actuel = noeud_dest;
        int titre = playlist_get_titre();
        ret = mp3player_set_track(titre);
        if(ret < 0){
            printf("erreur de lecture du titre %d", titre);
            return -1;
        }
        char *image = playlist_get_imgfile();
        ret = lcd_screen_display_from_file(image);
        if(ret < 0){
            printf("erreur à la lecture de l'image %s", image);
            return -1;
        }
        return 1;
    }
    return 0;
}

int playlist_get_parent(void){
    return arbre[noeud_actuel]->parent;
}
int playlist_get_enfant(void){
    printf("debug noeud_actuel %d\n\n", noeud_actuel);
    return arbre[noeud_actuel]->enfant;
}
int playlist_get_gauche(void){
    return arbre[noeud_actuel]->gauche;
}
int playlist_get_droit(void){
    return arbre[noeud_actuel]->droit;
}
char *playlist_get_imgfile(void){
    return arbre[noeud_actuel]->imgfile;
}
int playlist_get_audio(void){
    return arbre[noeud_actuel]->audio;
}
int playlist_get_titre(void){
    return arbre[noeud_actuel]->titre;
}
int playlist_init(void){

    arbre[1] = &accueil;
    arbre[2] = &belhist;
    arbre[3] = &colsven;
    arbre[4] = &voyext;

    noeud_actuel = 1;

    return noeud_actuel;
}

void playlist_display_noeud(int noeud){
    printf("noeud num %d\n", noeud);
    printf("parent %d, ", arbre[noeud]->parent);
    printf("enfant %d, ", arbre[noeud]->enfant);
    printf("gauche %d, ", arbre[noeud]->gauche);
    printf("droit %d\n", arbre[noeud]->droit);
    printf("imgfile %s\n", arbre[noeud]->imgfile);
    printf("titre %d\n", arbre[noeud]->titre);
    printf("audio %d\n\n", arbre[noeud]->audio);
}