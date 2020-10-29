#include "window.h"

void sayhello()
{
    printf("Hello!\n");
}

/*========================= INIT AL ==========================*/
void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

/*========================== MOUSE ===========================*/
int mx = 0;
int my = 0;
int mz = 0;
int mw = 0;
int mmx = 0;
int mmy = 0;
int mmz = 0;
int mmw = 0;
float p = 0.0;

// static int actual_buttons;
bool buttons[NUM_BUTTONS] = {false};

/*========================= BUTTONS ==========================*/
int bcount = 0;

void button_init(int x, int y, int w, int h, void (*f)(), char *text, ALLEGRO_COLOR color)
{
    wbuttons[bcount].x = x;
    wbuttons[bcount].y = y;
    wbuttons[bcount].w = w;
    wbuttons[bcount].h = h;
    wbuttons[bcount].active = true;
    wbuttons[bcount].pressed = false;
    wbuttons[bcount].f = f;
    wbuttons[bcount].text = text;
    wbuttons[bcount].color = color;
    bcount++;
}

void button_draw(ALLEGRO_FONT* font)
{
    for (int i = 0; i < bcount; i++)
    {
        WBUTTON thisb = wbuttons[i];
        al_draw_filled_rectangle(thisb.x, thisb.y, thisb.x + thisb.w,
            thisb.y + thisb.h, thisb.color);
        al_draw_text(font, al_map_rgb(0, 0, 0), thisb.x+(thisb.w-al_get_text_width(font, thisb.text))/2, 
            thisb.y+(thisb.h-al_get_font_line_height(font))/2, 0, thisb.text);
    }
}

/*========================== COMBO ===========================*/
void set_mode()
{
    if (combobox[0].active)
    {
        mode = MANUAL;
        combobox[0].active = false;
        combobox[0].color = al_map_rgb_f(1,1,1);//(149,195,224);
        combobox[1].active = true;
        combobox[1].color = al_map_rgb(168,63,0);
    }
    else
    {
        mode = AUTO;
        combobox[0].active = true;
        combobox[0].color = al_map_rgb(0,68,255);
        combobox[1].active = false;
        combobox[1].color = al_map_rgb_f(1,1,1);//(186,152,124);
    }
}

void combo_init(int x, int y, int w, int h)
{
    combobox[0].x = x;
    combobox[0].y = y;
    combobox[0].w = w/2-w/10;
    combobox[0].h = h;
    combobox[0].active = true;
    combobox[0].pressed = false;
    combobox[0].f = set_mode;
    combobox[0].text = "MANUAL";
    combobox[0].color = al_map_rgb(0,68,255);

    combobox[1].x = x+w/2+w/10;
    combobox[1].y = y;
    combobox[1].w = w/2-w/10;
    combobox[1].h = h;
    combobox[1].active = false;
    combobox[1].pressed = false;
    combobox[1].f = set_mode;
    combobox[1].text = "AUTO";
    combobox[1].color = al_map_rgb(255,255,255);
}

void combo_draw(ALLEGRO_FONT* font)
{
    for (int i = 0; i < 2; i++)
    {
        WBUTTON thisb = combobox[i];
        al_draw_filled_rectangle(thisb.x, thisb.y, thisb.x + thisb.w,
            thisb.y + thisb.h, thisb.color);
        al_draw_text(font, al_map_rgb(0, 0, 0), thisb.x+(thisb.w-al_get_text_width(font, thisb.text))/2, 
            thisb.y+(thisb.h-al_get_font_line_height(font))/2, 0, thisb.text);
    }
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
            for (int i = 0; i < bcount; i++)
                if (mx > wbuttons[i].x && mx < wbuttons[i].x + wbuttons[i].w && 
                    my > wbuttons[i].y && my < wbuttons[i].y + wbuttons[i].h) {
                        if (wbuttons[i].active)
                            wbuttons[i].f();
                }
            for (int i = 0; i < 2; i++)
                if (mx > combobox[i].x && mx < combobox[i].x + combobox[i].w && 
                    my > combobox[i].y && my < combobox[i].y + combobox[i].h) {
                        if (combobox[i].active == true)
                            combobox[i].f();
                }
            
            break;
    }
}