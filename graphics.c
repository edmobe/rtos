#include "window.h"

/*========================= INIT AL ==========================*/
void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

/*========================= DISPLAY ==========================*/
void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");
    al_set_window_title(disp, "RTOS - The future in scheduling");
}

void disp_deinit()
{
    al_destroy_display(disp);
}

/*======================== KEYBOARD ==========================*/
void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch(event->type)
    {
        case ALLEGRO_EVENT_TIMER:
            for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event->keyboard.keycode] &= KEY_RELEASED;
            break;
    }
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
        if (thisb.active) {
            al_draw_filled_rectangle(thisb.x, thisb.y, thisb.x + thisb.w,
                thisb.y + thisb.h, thisb.color);
            al_draw_text(font, al_map_rgb(0, 0, 0), thisb.x+(thisb.w-al_get_text_width(font, thisb.text))/2, 
                thisb.y+(thisb.h-al_get_font_line_height(font))/2, 0, thisb.text);
        }
    }
}

/*========================== MODE ============================*/

void ambientmode()
{
    if(key[ALLEGRO_KEY_X])
    {
        running = false;
        wbuttons[0].pressed = false;
        wbuttons[0].color = al_map_rgb_f(0,1,0);
        wbuttons[0].text = "Start";
        combobox[0].active = true;
        combobox[1].active = true;
    }
    if (mode == MANUAL) {
        if (running)
            wbuttons[1].active = true;
        else
            wbuttons[1].active = false;
    } else {
        if (running)
            wbuttons[1].active = false;
        else
            wbuttons[1].active = true;
    }
}

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

/*========================== COMBO ===========================*/

void combo_init(int x, int y, int w, int h)
{
    combobox[0].x = x;
    combobox[0].y = y;
    combobox[0].w = w/2-w/12;
    combobox[0].h = h;
    combobox[0].active = true;
    combobox[0].pressed = false;
    combobox[0].f = set_mode;
    combobox[0].text = "MANUAL";
    combobox[0].color = al_map_rgb(0,68,255);

    combobox[1].x = x+w/2+w/12;
    combobox[1].y = y;
    combobox[1].w = w/2-w/12;
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

/*========================== CLICK ===========================*/
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
                        if (wbuttons[i].active && !wbuttons[i].pressed)
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

/*======================== TEXT AREA ==========================*/
void textarea_init()
{

}

void textarea_draw(ALLEGRO_FONT* font)
{
    if (mode == AUTO) {
        
        al_draw_rectangle(1,(DISP_H/24)*7+10,(DISP_W-DISP_H)/2-6, (DISP_H/24)*20, al_map_rgb(255,255,255), 2);
    }
}

/*======================== DATA INPUT =========================*/
void incrementenergy ()
{
    energy++;
}
void decrementenergy ()
{   
    if (energy > 0)
        energy--;
}
void incrementperiod ()
{
    period++;
}
void decrementperiod ()
{
    if (period > 0)
        period--;
}

void datainput_init(ALLEGRO_FONT* font)
{
    button_init((DISP_W-DISP_H)/2*2/3 - 20, (DISP_H/24)*20+4,(DISP_W-DISP_H)/6*1/3, al_get_font_line_height(font)+4, 
        incrementenergy, "+", al_map_rgb(180, 180, 180));
    button_init((DISP_W-DISP_H)/2*2/3 + al_get_font_line_height(font) + 20, (DISP_H/24)*20+4,(DISP_W-DISP_H)/6*1/3, al_get_font_line_height(font)+4, 
        decrementenergy, "-", al_map_rgb(180, 180, 180));
    button_init((DISP_W-DISP_H)/2*2/3 - 20, (DISP_H/24)*21+4,(DISP_W-DISP_H)/6*1/3, al_get_font_line_height(font)+4, 
        incrementperiod, "+", al_map_rgb(180, 180, 180));
    button_init((DISP_W-DISP_H)/2*2/3 + al_get_font_line_height(font) + 20, (DISP_H/24)*21+4,(DISP_W-DISP_H)/6*1/3, al_get_font_line_height(font)+4, 
        decrementperiod, "-", al_map_rgb(180, 180, 180));
}

void datainput_update()
{
    for (int i = 2; i < bcount; i++) {
        if (mode == AUTO || (mode == MANUAL && running)) {
            wbuttons[i].active = true;
        } else {
            wbuttons[i].active = false;
        }
    }
}

void datainput_draw(ALLEGRO_FONT* font)
{
    if (mode == AUTO || (mode == MANUAL && running)) {
        al_draw_text(font, al_map_rgb_f(1,1,1),3,(DISP_H/24)*20+8,0,"ENERGY:");
        al_draw_rectangle(13+al_get_text_width(font,"ENERGY:"),(DISP_H/24)*20+6,(DISP_W-DISP_H)/2*1/3,
            (DISP_H/24)*20+10+al_get_font_line_height(font),al_map_rgb(255,255,255),1);
        al_draw_textf(font, al_map_rgb(0,248,68),33+al_get_text_width(font,"ENERGY:")+2,(DISP_H/24)*20+8,
            0,"%d", energy);
        al_draw_text(font, al_map_rgb_f(1,1,1),3,(DISP_H/24)*21+8,0,"PERIOD:");
        al_draw_rectangle(13+al_get_text_width(font,"PERIOD:"),(DISP_H/24)*21+6,(DISP_W-DISP_H)/2*1/3,
            (DISP_H/24)*21+10+al_get_font_line_height(font),al_map_rgb(255,255,255),1);
        al_draw_textf(font, al_map_rgb(0,75,212),33+al_get_text_width(font,"ENERGY:")+2,(DISP_H/24)*21+8,
            0,"%d", period);
    } 
}

void startlogic()
{
    running = true;
    wbuttons[0].pressed = true;
    wbuttons[0].color = al_map_rgb_f(1,0,0);
    wbuttons[0].text = "Running";
    combobox[0].active = false;
    combobox[1].active = false;
}