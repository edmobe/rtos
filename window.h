#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

enum {
    MANUAL = 1,
    AUTO
};

static int mode;
static bool done;
static bool redraw;

void must_init(bool test, const char *description);

/*========================= DISPLAY ==========================*/
// #define BUFFER_W 320
// #define BUFFER_H 240

// #define DISP_SCALE 3
#define DISP_W 1280//(BUFFER_W * DISP_SCALE)
#define DISP_H 720//(BUFFER_H * DISP_SCALE)

/*========================== MOUSE ===========================*/
#define NUM_BUTTONS 5 // in a mouse
#define BUTTONS_MAX 4 // buttons widgets in GUI

static int actual_buttons;
// bool buttons[NUM_BUTTONS] = {false};

/*========================= BUTTONS ==========================*/
typedef struct WBUTTON
{
    int x, y;
    int w, h;
    bool active;
    bool pressed;
    void (*f)();
    char *text;
    ALLEGRO_COLOR color;
} WBUTTON;

WBUTTON wbuttons[BUTTONS_MAX];
WBUTTON combobox[2];
// int bcount = 0;

// void mouse_init();

void button_init(int x, int y, int w, int h, void (*f)(), char *text, ALLEGRO_COLOR color);

void click_update(ALLEGRO_EVENT* event);

void button_draw(ALLEGRO_FONT *font);

void sayhello();

/*========================== COMBO ===========================*/
void set_mode();

void combo_init(int x, int y, int w, int h);

void combo_draw(ALLEGRO_FONT* font);