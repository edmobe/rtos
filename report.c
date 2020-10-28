#include "report.h"

typedef struct BLOCK {
    char id[10];
    int duration;
    int period;
    int current_number;

} BLOCK;




int main (){


    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    BLOCK mar1;
    mar1.current_number = 0;
    mar1.duration = 10;
    mar1.period = 5;
    strcpy(mar1.id, "Brayan");


    ALLEGRO_DISPLAY* pDisplay = al_create_display(ALTO, ANCHO);
    ALLEGRO_EVENT_QUEUE* oQueue;
    ALLEGRO_FONT* font = al_load_font("_decterm.ttf", 15, 0);

    oQueue = al_create_event_queue();
    al_register_event_source(oQueue, al_get_keyboard_event_source());
    al_register_event_source(oQueue, al_get_display_event_source(pDisplay));

    char *sWindowTitle = "Marcianitos";
    al_set_window_title(pDisplay, sWindowTitle);  

    int x = 0, y = 0, move = 5;
    bool done = false;
    // Message box
    while(!done) {
        ALLEGRO_EVENT oEvent;
        gen_dividers();
        gen_marciano(mar1, font);
        al_flip_display();
        al_rest(0.03);
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


void gen_dividers () {
    int current = 40;
    while (current < ALTO){
        al_draw_line(current, 50, current, ANCHO - 50, al_map_rgb(255,255,255), 1.0);
        current += TIMEDIVIDERS;
    }
}

void gen_marciano (BLOCK marciano, ALLEGRO_FONT *font){
    int current_x = 40, current_y = 0;
    
    al_draw_filled_rectangle(45, 60, current_x + TIMEDIVIDERS - 5, 90, al_map_rgb(140,250,120));
    al_draw_text(font, al_map_rgb(140,250,120), ANCHO / 2, ALTO / 2, ALLEGRO_ALIGN_CENTER, marciano.id);
}