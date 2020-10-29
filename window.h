#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

/*========================== MOUSE ===========================*/
#define NUM_BUTTONS 5 // in a mouse
#define BUTTONS_MAX 4 // buttons widgets in GUI

// int mx = 0;
// int my = 0;
// int mz = 0;
// int mw = 0;
// int mmx = 0;
// int mmy = 0;
// int mmz = 0;
// int mmw = 0;
// bool in = true;
// float p = 0.0;

static int actual_buttons;
static bool in;
// bool buttons[NUM_BUTTONS] = {false};

typedef struct WBUTTON
{
    int x, y;
    int w, h;
    bool active;
    bool pressed;
    void (*f)();
    ALLEGRO_COLOR color;
} WBUTTON;

WBUTTON wbuttons[BUTTONS_MAX];
// int bcount = 0;

void must_init(bool test, const char *description);

// void mouse_init();

void button_init(int x, int y, int w, int h, void (*f)(), ALLEGRO_COLOR color);

void click_update(ALLEGRO_EVENT* event);

void button_draw();

void sayhello();