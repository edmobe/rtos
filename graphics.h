#include <stdbool.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>

void must_init(bool test, const char *description);

/*========================== MOUSE ===========================*/
#define NUM_BUTTONS 5 // in a mouse
#define BUTTONS_MAX 4 // buttons widgets in GUI

void mouse_init();

void button_init(int x, int y, int w, int h, void (*f)(), ALLEGRO_COLOR color);

void click_update(ALLEGRO_EVENT* event);

void button_draw();