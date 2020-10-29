#include "report.h"


int main (){

    // ----------------
    // Initialization
    // ----------------

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();


    srand(time(NULL));

    // -----------
    // Variables
    // -----------
    float cameraPosition[2] = { 0, 0};
    float x = 0, y = 0;
    // --------
    // Aliens
    // --------

    BLOCK mar1;
    mar1.current_number = 0;
    mar1.duration = 1;
    mar1.period = 6;
    mar1.color = RandomColor();
    strcpy(mar1.id, "A");
    
    BLOCK mar2;
    mar2.current_number = 0;
    mar2.duration = 2;
    mar2.period = 9;
    mar2.color = RandomColor();
    strcpy(mar2.id, "B");

    BLOCK mar3;
    mar3.current_number = 0;
    mar3.duration = 6;
    mar3.period = 18;
    mar3.color = RandomColor();
    strcpy(mar3.id, "C");

    BLOCK mar4;
    mar4.current_number = 0;
    mar4.duration = 5;
    mar4.period = 10;
    mar4.color = RandomColor();
    strcpy(mar4.id, "D");

    // ------------------
    // Allegro elements
    // ------------------

    ALLEGRO_DISPLAY* pDisplay = al_create_display(ANCHO, ALTO);
    ALLEGRO_EVENT_QUEUE* oQueue;
    ALLEGRO_FONT* font_small = al_load_font(FONTNAME, 10, 0);
    ALLEGRO_FONT* font = al_load_font(FONTNAME, 15, 0);
    ALLEGRO_FONT* font_big = al_load_font(FONTNAME, 30, 0);
    
    
    // --------
    // Events
    // --------

    oQueue = al_create_event_queue();
    al_register_event_source(oQueue, al_get_keyboard_event_source());
    al_register_event_source(oQueue, al_get_display_event_source(pDisplay));

    // ------------
    // Properties
    // ------------

    char *sWindowTitle = "Marcianitos";
    al_set_window_title(pDisplay, sWindowTitle);  
    al_set_window_position(pDisplay, 300, 50);

    bool done = false;


    // -----------------
    // Generate Blocks
    // -----------------

    GenDividers(WidthDivider, font);
    GenAlien(mar1, font);
    GenAlien(mar2, font);
    GenAlien(mar3, font);
    GenAlien(mar4, font);

    al_draw_text(font_big, al_map_rgb(255,255,255), ANCHO / 2, 10, ALLEGRO_ALIGN_CENTER, TITLE);
    al_draw_text(font_small, al_map_rgb(255,255,255), 20, 30, ALLEGRO_ALIGN_CENTER, "Procesos");

    // -------
    // Bucle
    // -------
    
    while(!done) {

        ALLEGRO_EVENT oEvent;

        
        
        al_flip_display();
        al_rest(0.03);

        // -----------------
        //  Events waiting
        // -----------------

        al_wait_for_event(oQueue, &oEvent);

        switch(oEvent.keyboard.keycode) {

        case ALLEGRO_KEY_X:
            done = true;
        break;

        case ALLEGRO_KEY_LEFT:
            x -= SPEED;
        break;
        
        case ALLEGRO_KEY_RIGHT:
            x += SPEED;
        break;

        case ALLEGRO_KEY_DOWN:
            y += SPEED;
        

        case ALLEGRO_KEY_UP:
            y -= SPEED;
        break;

        default:

        break;      
        }
        //CameraUpdate(cameraPosition, x, y,32,32);
        
    }    
    al_destroy_event_queue(oQueue);
    al_destroy_display(pDisplay);
    al_destroy_font(font);
    al_destroy_font(font_big);
    al_destroy_font(font_small);
    return 0;
    
}

// ---------------------
// Dividers generation
// ---------------------

void GenDividers (int min, ALLEGRO_FONT *font) {
    int current = 40, time_current = 0;
    char current_num[10];

    while (current < ANCHO){
        sprintf(current_num, "%d", time_current);
        al_draw_line(current, 50, current, ALTO - 50, al_map_rgb(255,255,255), 1.0);
        al_draw_text(font, al_map_rgb(255,255,255), current, ALTO - 40, ALLEGRO_ALIGN_CENTER, current_num);
        current += min * 30;
        time_current += min;
    }
}

// ----------------------
// Generate alien block
// ----------------------

void GenAlien (BLOCK marciano, ALLEGRO_FONT *font){

    int current_x = 40, current_y = current_height;;
    int period = 0, it = 0;
    char num[10], id[2];
    al_draw_text(font, al_map_rgb(255,255,255), 20, (current_y*2 + 30) / 2 - 5, ALLEGRO_ALIGN_CENTER, marciano.id);

    while (current_x < ANCHO) {
        if ((period % marciano.period) == 0){
            strcpy(id, marciano.id);
            sprintf(num, "%d", it);
            
            al_draw_filled_rectangle(current_x + 2, current_y, (current_x + marciano.duration * S1) - 2 , current_y + S1, marciano.color);
            al_draw_text(font, al_map_rgb(0,0,0), current_x + marciano.duration * S1 / 2, current_y + 10, ALLEGRO_ALIGN_CENTER, strcat(id,num));
            it ++;
        }
        period ++;
        current_x += WidthDivider * 30;
        
    }

    current_height += 40;
    // Position (y1 + y2 / 2) - 5
    
}

// -----------------------
// Set alien block color
// -----------------------

ALLEGRO_COLOR RandomColor () {
    return al_map_rgb (rand() % 255, rand() % 255, rand() % 255);
}

// --------------
// Camera moves
// --------------

void CameraUpdate (float *cameraPosition, float x, float y, int width, int height){
    cameraPosition[0] = - (ANCHO / 2) + (x + width / 2);
    cameraPosition[1] = - (ALTO / 2) + (y + height / 2);

    if (cameraPosition[0] < 0)
        cameraPosition[0] = 0;
    if (cameraPosition[1] < 0)
        cameraPosition[1] = 0;
}