#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>


const int ALTO = 600;
const int ANCHO = 800;


const float DELAY = 0.1;
const int TIMEDIVIDERS = 100;
const int HEIGHTROW = 20;

const char *FONTNAME = "_decterm.ttf";
const char *TITLE = "Reporte";


typedef struct BLOCK {
    char id[10];
    int duration;
    int period;
    int current_number;
    ALLEGRO_COLOR color;

} BLOCK;



void gen_dividers (int min, ALLEGRO_FONT *font);
void gen_marciano ();
void gen_report ();
ALLEGRO_COLOR random_color ();