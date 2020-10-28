#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

const int ALTO = 800;
const int ANCHO = 600;
const float DELAY = 0.1;
const int TIMEDIVIDERS = 100;
const int HEIGHTROW = 20;

void gen_dividers ();
void gen_marciano ();
void gen_report ();