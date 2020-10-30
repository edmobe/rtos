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

typedef struct BLOCK {
    char id[10];
    int duration;
    int period;
    int current_number;
    ALLEGRO_COLOR color;

} BLOCK;

void GenDividers (int min, ALLEGRO_FONT *font, int reportIteration);
void GenAlien (BLOCK *marciano, ALLEGRO_FONT *font, int alienLength, int reportIteration);
void GenAlgoReport (int *order, BLOCK *marciano, ALLEGRO_FONT *font, char *text, int reportIterations);
void Report(BLOCK *aliens, int *log, int alienLength, int reportIterations, char *method);
ALLEGRO_COLOR RandomColor ();

void CameraUpdate (float *cameraPosition, float x, float y, int width, int height, int reportIteration);

#endif