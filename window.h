#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

#include "mainlogic.h"

enum {
    MANUAL = 1,
    AUTO
};

#define RM 10
#define EDF 11

int mode;
int algorithm;
bool done;
bool redraw;
bool running;
long frames;

void must_init(bool test, const char *description);

/*========================= DISPLAY ==========================*/
// #define BUFFER_W 320
// #define BUFFER_H 240

// #define DISP_SCALE 3
#define DISP_W 1280//(BUFFER_W * DISP_SCALE)
#define DISP_H 720//(BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;

void disp_init();

void disp_deinit();

/*========================= SPRITES ==========================*/
#define ALIEN_W 25
#define ALIEN_H 21

typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;
    ALLEGRO_BITMAP* alien_down;
    ALLEGRO_BITMAP* alien_up;
    ALLEGRO_BITMAP* alien_right;
    ALLEGRO_BITMAP* alien_left;
} SPRITES;
SPRITES sprites;

ALLEGRO_COLOR bitmap_colors[MAX_PROCESSES];

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);

void sprites_init();

void sprites_deinit();

/*======================== KEYBOARD ==========================*/
#define KEY_SEEN     1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init();

void keyboard_update(ALLEGRO_EVENT* event);

/*========================== MOUSE ===========================*/
#define NUM_BUTTONS 5 // in a mouse
#define BUTTONS_MAX 8 // buttons widgets in GUI

int actual_buttons;
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

// void mouse_init();

void button_init(int x, int y, int w, int h, void (*f)(), char *text, ALLEGRO_COLOR color);

void button_draw(ALLEGRO_FONT *font);

/*========================== COMBO ===========================*/
void set_mode();

void combo_init(int x, int y, int w, int h);

void combo_draw(ALLEGRO_FONT* font);

/*========================== CLICK ===========================*/
void click_update(ALLEGRO_EVENT* event);

void mode_update();

/*======================== TEXT AREA =========================*/
void textarea_init();

void textarea_draw(ALLEGRO_FONT* font);

/*======================= DATA INPUT =========================*/
int energy;
int period;

void datainput_init(ALLEGRO_FONT* font);

void datainput_update();

void datainput_draw(ALLEGRO_FONT* font);

void add_alien();

/*========================= RUNNING ==========================*/
void startlogic();

void start_update();

/*=========================== MAZE ===========================*/

#define MAZE_W 720
#define MAZE_H 720
#define MBLOCK (MAZE_W / DIMENSIONS)

void maze_init();

void maze_update();

void maze_draw();