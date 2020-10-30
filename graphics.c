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

/*======================== SPRITES ===========================*/
ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("data/alienspritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    sprites.alien_down = sprite_grab(3, 6, ALIEN_W, ALIEN_H);
    sprites.alien_right = sprite_grab(5, 36, ALIEN_H, ALIEN_W);
    sprites.alien_left = sprite_grab(5, 68, ALIEN_H, ALIEN_W);
    sprites.alien_up = sprite_grab(3, 100, ALIEN_W, ALIEN_H);

}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.alien_down);
    al_destroy_bitmap(sprites.alien_right);
    al_destroy_bitmap(sprites.alien_left);
    al_destroy_bitmap(sprites.alien_up);

    al_destroy_bitmap(sprites._sheet);
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
        case ALLEGRO_EVENT_KEY_CHAR:
            if (event->keyboard.keycode == ALLEGRO_KEY_C)
            {
                if (algorithm == EDF)
                    algorithm = RM;
                else
                    algorithm = EDF;
            }
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
            al_draw_text(font, al_map_rgb(0, 0, 0), thisb.x+thisb.w/2, 
                thisb.y+(thisb.h-al_get_font_line_height(font))/2, 
                    ALLEGRO_ALIGN_CENTER, thisb.text);
        }
    }
}

/*========================== MODE ============================*/

void mode_update()
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
    time_t t;
   
    /* Intializes random number generator */
    srand((unsigned) time(&t));
    for (int i = 0; i < MAX_PROCESSES; i++){
        if (i == 0)
            bitmap_colors[i] = al_map_rgb(0,255,0);
        else
            bitmap_colors[i] = al_map_rgb(rand()%256, rand()%256, rand()%256);
    }
}

void textarea_draw(ALLEGRO_FONT* font)
{
    if (mode == AUTO || (mode == MANUAL && running)) {
        al_draw_rectangle(1,(DISP_H/24)*7+5,(DISP_W-DISP_H)/2-6, 
            (DISP_H/24)*20, al_map_rgb(255,255,255), 2);
        int counter = 0;
        
        for (int i = 7; counter < alienArray.length; i=i+2) {
            al_draw_rectangle(10,(DISP_H/24)*i+10,(DISP_W-DISP_H)/2-15, 
                (DISP_H/24)*(i+2)+5, al_map_rgb(255,255,255), 2);
            float temp = ((DISP_H/24)*i+10) + (((DISP_H/24)*(i+2)+5) - ((DISP_H/24)*i+10))/2;
            if (counter == 0)
                al_draw_bitmap(sprites.alien_down, 15, temp-10, 0);
            else
                al_draw_tinted_bitmap(sprites.alien_down, bitmap_colors[counter] ,15 , temp-10, 0);
            al_draw_textf(font, al_map_rgb_f(1,1,1), 55, temp-20, 0, "ALIEN %d", counter+1);
            al_draw_textf(font, al_map_rgb_f(1,1,1), 55, temp+5, 0, "ENERGY: %d", 
                alienArray.aliens[counter].energy);
            al_draw_textf(font, al_map_rgb_f(1,1,1), 125, temp+5, 0, "PERIOD: %d", 
                alienArray.aliens[counter].period);
            counter++;
        }
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

void add_alien()
{
    if (period <= energy) {
        printf("The period needs to be greater then the energy.\n");
        return;
    } 
    if (energy == 0 || period == 0) {
        printf("The values can't be cero.\n");
        return;       
    }
    append(energy, period, 0);
    energy = 0;
    period = 0;
}

/*========================= RUNNING ==========================*/

void startlogic()
{
    running = true;
    wbuttons[0].pressed = true;
    wbuttons[0].color = al_map_rgb_f(1,0,0);
    wbuttons[0].text = "Running";
    combobox[0].active = false;
    combobox[1].active = false;
    iterationCounter = 0;
    if (algorithm == EDF)
        edf(iterationCounter);
    else
        rm(iterationCounter);
    internthread = alienArray.higherPriorityIndex;
}

void start_update()
{
    if ((iterationCounter>=100 || finished) && !running) {
        // Generate report here
        printf("REPORTE\n");
        return;
    }

    if (frames >= 30) {
        frames = 0;
        if (algorithm == EDF)
            edf(iterationCounter);
        else
            rm(iterationCounter);
        internthread = alienArray.higherPriorityIndex;
        iterationCounter++;
    }
}

/*=========================== MAZE ===========================*/
void maze_init()
{

}

void maze_update()
{

}

void maze_draw()
{
    for (int i = 0; i < DIMENSIONS; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            if (MAZE[i][j] == 1) {
                al_draw_rectangle((DISP_W-DISP_H)/2+MBLOCK*j+4,MBLOCK*i+4,
                    (DISP_W-DISP_H)/2+MBLOCK*(j+1)-4, MBLOCK*(i+1)-4,al_map_rgb(143, 35, 48), 4);
            }
        }
    }
    for (int i = 0; i < alienArray.length; i++) {
        if (alienArray.aliens[i].posX != 0 || alienArray.aliens[i].posY != 0) {
            int thisx = alienArray.aliens[i].posX;
            int thisy = alienArray.aliens[i].posY;
            ALLEGRO_BITMAP* thissprite;
            switch (alienArray.aliens[i].direction) {
                case 'r':
                    thissprite = sprites.alien_right;
                    break;
                case 'l':
                    thissprite = sprites.alien_left;
                    break;
                case 'u':
                    thissprite = sprites.alien_up;
                    break;
                case 'd':
                    thissprite = sprites.alien_down;
                    break;
            }
            if (i == 0) 
                al_draw_bitmap(thissprite, (DISP_W-DISP_H)/2+MBLOCK*thisx+6, MBLOCK*thisy+6, 0);
            else
                al_draw_tinted_bitmap(thissprite, bitmap_colors[i],(DISP_W-DISP_H)/2+MBLOCK*thisx+6, 
                    MBLOCK*thisy+6, 0);
        }
    }
}

/*========================= ENERGY ============================*/
void energy_draw(ALLEGRO_FONT* font) {
    if (running && internthread != -1){
        int thisener = alienArray.remainingEnergies[internthread];
        for (int i = 0; i < thisener; i ++) {
            float xsp = DISP_W-(DISP_W-DISP_H)/2+10+al_get_text_width(font, "Energy Level:") + 12 * i;
            al_draw_filled_rectangle( xsp, DISP_H/24, xsp + 10, DISP_H/24+al_get_font_line_height(font), 
                al_map_rgb_f(1,1,1));
        }
    }
}

/*===================== MOVING ALIEN ==========================*/
void moving_draw(ALLEGRO_FONT* font)
{
    if (running && internthread != -1){
        if (internthread == 0)
            al_draw_bitmap(sprites.alien_down, DISP_W-(DISP_W-DISP_H)/4-ALIEN_W/2, DISP_H/2 + 40, 0);
        else
            al_draw_tinted_bitmap(sprites.alien_down, bitmap_colors[internthread],
                 DISP_W-(DISP_W-DISP_H)/4-ALIEN_W/2, DISP_H/2 + 40, 0);
        al_draw_textf(font, al_map_rgb_f(1,1,1), DISP_W-(DISP_W-DISP_H)/4, DISP_H/2 + 100, 
            ALLEGRO_ALIGN_CENTER, "ALIEN %d", internthread+1);
    }

}