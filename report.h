#ifndef HEADER_FILE
#define HEADER_FILE

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int HEIGHT = 600;
const int WIDTH = 800;
const int WidthDivider = 1;

const float DELAY = 0.1;
const int TIMEDIVIDERS = 100;
const int HEIGHTROW = 60;
const char *FONTNAME = "fonts/_decterm.ttf";
const char *TITLE = "Reporte generado";
const int SPEED = 10;
const int FPS = 60;

const int S1 = 30;
const int S2 = 15;
int current_height = HEIGHTROW;

typedef struct BLOCK {
    char id[10];
    int duration;
    int period;
    int current_number;
    ALLEGRO_COLOR color;

} BLOCK;



#endif