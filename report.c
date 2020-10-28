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
    srand(getpid());

    // --------
    // Aliens
    // --------

    BLOCK mar1;
    mar1.current_number = 0;
    mar1.duration = 10;
    mar1.period = 5;
    mar1.color = random_color();
    strcpy(mar1.id, "B");
    

    // ------------------
    // Allegro elements
    // ------------------

    ALLEGRO_DISPLAY* pDisplay = al_create_display(ANCHO, ALTO);
    ALLEGRO_EVENT_QUEUE* oQueue;
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

    int x = 0, y = 0, move = 5;
    bool done = false;

    // -------
    // Bucle
    // -------

    while(!done) {

        ALLEGRO_EVENT oEvent;

        gen_dividers(30, font);
        gen_marciano(mar1, font);
        
        al_draw_text(font_big, al_map_rgb(255,255,255), ANCHO / 2, 10, ALLEGRO_ALIGN_CENTER, TITLE);
        
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
        default:
        
        break;      
        }
    }    

    return 0;
    
}

// ---------------------
// Dividers generation
// ---------------------

void gen_dividers (int min, ALLEGRO_FONT *font) {
    int current = 40, time_current = 0;
    char current_num[10];

    while (current < ANCHO){
        sprintf(current_num, "%d", time_current);
        al_draw_line(current, 50, current, ALTO - 50, al_map_rgb(255,255,255), 1.0);
        al_draw_text(font, al_map_rgb(255,255,255), current - 3, ALTO - 40, ALLEGRO_ALIGN_CENTER, current_num);
        current += min;
        time_current += min;
    }
}

// ----------------------
// Generate alien block
// ----------------------

void gen_marciano (BLOCK marciano, ALLEGRO_FONT *font){
    int current_x = 40, current_y = 0;
    al_draw_text(font, al_map_rgb(255,255,255), 20, 70, ALLEGRO_ALIGN_CENTER, marciano.id);

    
    al_draw_filled_rectangle(45, 60, current_x + TIMEDIVIDERS - 5, 90, marciano.color);
    // Position (y1 + y2 / 2) - 5
    
}

// -----------------------
// Set alien block color
// -----------------------

ALLEGRO_COLOR random_color () {
    return al_map_rgb (rand() % 255, rand() % 255, rand() % 255);
}