#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "graphics.h"

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int mx = 0;
int my = 0;
int mz = 0;
int mw = 0;
int mmx = 0;
int mmy = 0;
int mmz = 0;
int mmw = 0;
bool in = true;
float p = 0.0;

static int actual_buttons;
bool buttons[NUM_BUTTONS] = {false};

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
int bcount = 0;

void mouse_init()
{
    actual_buttons = al_get_mouse_num_buttons();
    if (actual_buttons > NUM_BUTTONS)
        actual_buttons = NUM_BUTTONS;
}

void button_init(int x, int y, int w, int h, void (*f)(), ALLEGRO_COLOR color)
{
    wbuttons[bcount].x = x;
    wbuttons[bcount].y = y;
    wbuttons[bcount].w = w;
    wbuttons[bcount].h = h;
    wbuttons[bcount].active = true;
    wbuttons[bcount].pressed = false;
    wbuttons[bcount].f = f;
    wbuttons[bcount].color = color;
    bcount++;
}

void click_update(ALLEGRO_EVENT* event)
{
    switch(event->type)
    {
        
        case ALLEGRO_EVENT_MOUSE_AXES:
            mx = event->mouse.x;
            my = event->mouse.y;
            mz = event->mouse.z;
            mw = event->mouse.w;
            mmx = event->mouse.dx;
            mmy = event->mouse.dy;
            mmz = event->mouse.dz;
            mmw = event->mouse.dw;
            p = event->mouse.pressure;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event->mouse.button-1 < NUM_BUTTONS) {
                buttons[event->mouse.button-1] = true;
            }
            p = event->mouse.pressure;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (event->mouse.button-1 < NUM_BUTTONS) {
                buttons[event->mouse.button-1] = false;
            }
            p = event->mouse.pressure;
            if (mx > wbuttons[0].x && mx < wbuttons[0].x + wbuttons[0].w && 
                my > wbuttons[0].y && my < wbuttons[0].y + wbuttons[0].h) {
                    wbuttons[0].pressed = true;
                    wbuttons[0].f();
            }
            break;

        case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                in = true;
                break;

            case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
                in = false;
                break;
    }
}

void button_draw()
{
    ALLEGRO_COLOR grey;
    ALLEGRO_COLOR black;
    for (int i = 0; i < bcount; i++)
    {
        al_draw_filled_rectangle(wbuttons[i].x, wbuttons[i].y, wbuttons[i].x + wbuttons[i].w,
            wbuttons[i].y + wbuttons[i].h, wbuttons[i].color);
    }
}