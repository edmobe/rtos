#include "report.h"

void GenDividers (int min, ALLEGRO_FONT *font);
void GenAlien (BLOCK *marciano, ALLEGRO_FONT *font, int alienLength);
void GenAlgoReport (int *order, BLOCK *marciano, ALLEGRO_FONT *font, char *text, int reportIterations);
void Report ();
ALLEGRO_COLOR RandomColor ();

void CameraUpdate (float *cameraPosition, float x, float y, int width, int height);

void Report(BLOCK *aliens, int *log, int alienLength, int reportIterations){

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
    /*
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

    BLOCK aliens[4] = {mar1, mar2, mar3, mar4};

    int order[10] = {0,1,0,1,1,-1,2,2,2,3};
    */
    // ------------------
    // Allegro elements
    // ------------------

    ALLEGRO_DISPLAY* pDisplay = al_create_display(WIDTH, HEIGHT);
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

    char *sWindowTitle = "Reporte";
    al_set_window_title(pDisplay, sWindowTitle);  
    al_set_window_position(pDisplay, 50, 30);

    bool done = false;

    GenDividers(WidthDivider, font);

    // -----------------
    // Generate Blocks
    // -----------------
    
    
    GenAlien(aliens, font, alienLength);
    GenAlgoReport(log, aliens, font, "EDF", reportIterations);
    
    al_draw_text(font_big, al_map_rgb(255,255,255), WIDTH / 2, 10, ALLEGRO_ALIGN_CENTER, TITLE);
    al_draw_text(font_small, al_map_rgb(255,255,255), 20, 30, ALLEGRO_ALIGN_CENTER, "Procesos");

    // -------
    // Bucle
    // -------
    al_flip_display();
    al_rest(0.03);
    while(!done) {

        ALLEGRO_EVENT oEvent;

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
    al_destroy_event_queue(oQueue);
    al_destroy_display(pDisplay);
    al_destroy_font(font);
    al_destroy_font(font_big);
    al_destroy_font(font_small);
}

// ---------------------
// Dividers generation
// ---------------------

void GenDividers (int min, ALLEGRO_FONT *font) {
    int current = 40, time_current = 0;
    char current_num[10];

    while (current < WIDTH){
        sprintf(current_num, "%d", time_current);
        al_draw_line(current, 50, current, HEIGHT - 50, al_map_rgb(255,255,255), 1.0);
        al_draw_text(font, al_map_rgb(255,255,255), current, HEIGHT - 40, ALLEGRO_ALIGN_CENTER, current_num);
        current += S1;
        time_current += min;
    }
}

// ----------------------
// Generate alien block
// ----------------------

void GenAlien (BLOCK *marciano, ALLEGRO_FONT *font, int alienLength){
    for (int i = 0; i < alienLength; i++){
        printf("%s. %d %d\n", marciano[i].id, marciano[i].duration, marciano[i].period);
        int current_x = 40, current_y = current_height;
        int period = 0, it = 0;
        char num[10], id[2];
        al_draw_text(font, al_map_rgb(255,255,255), 20, (current_y*2 + 30) / 2 - 5, ALLEGRO_ALIGN_CENTER, marciano[i].id);
        marciano[i].color = RandomColor();
        while (current_x < WIDTH) {
            if ((period % marciano[i].period) == 0){
                strcpy(id, marciano[i].id);
                sprintf(num, "%d", it);
                
                al_draw_filled_rectangle(current_x, current_y, (current_x + marciano[i].duration * S1 / WidthDivider) , current_y + S1, marciano[i].color);
                if (marciano[i].duration < WidthDivider){
                    al_draw_text(font, al_map_rgb(255,255,255), current_x + marciano[i].duration * (S1 / WidthDivider) / 2, current_y + 10, ALLEGRO_ALIGN_CENTER, strcat(id,num));    
                }
                else {
                    al_draw_text(font, al_map_rgb(0,0,0), current_x + marciano[i].duration * (S1 / WidthDivider) / 2, current_y + 10, ALLEGRO_ALIGN_CENTER, strcat(id,num));
                }
                
                it ++;
            }
            period ++;
            current_x += S1 / WidthDivider;
            
        }

        current_height += 40;
    }
    
    // Position (y1 + y2 / 2) - 5
    
}

// -----------------------
// Generate EDF or RMS
// -----------------------
void GenAlgoReport (int *order, BLOCK *marciano, ALLEGRO_FONT *font, char *text, int reportIterations){
    
    int current_x = 40, current_y = current_height;
    int period = 0, it = 0;
    al_draw_text(font, al_map_rgb(255,255,255), 20, (current_y*2 + 30) / 2 - 5, ALLEGRO_ALIGN_CENTER, text);
    char num[10], id[2];
    for (int i = 0; i < reportIterations; i++) {
        if (order[i] != -1){
            strcpy(id, marciano[order[i]].id);
            sprintf(num, "%d", it);
            
            al_draw_filled_rectangle(current_x, current_y, (current_x + S1 / WidthDivider) , current_y + S1, marciano[order[i]].color);
            if (WidthDivider > 1){
                al_draw_text(font, al_map_rgb(255,255,255), current_x + (S1 / WidthDivider) / 2, current_y + 10, ALLEGRO_ALIGN_CENTER, marciano[order[i]].id);
            }
            else {
                al_draw_text(font, al_map_rgb(0,0,0), current_x + (S1 / WidthDivider) / 2, current_y + 10, ALLEGRO_ALIGN_CENTER, marciano[order[i]].id);
            }
        }
        
        current_x += S1 / WidthDivider;
    }
}

// -----------------------
// Set alien block color
// -----------------------

ALLEGRO_COLOR RandomColor () {
    return al_map_rgb (rand() % 256, rand() % 256, rand() % 256);
}

// --------------
// Camera moves
// --------------

void CameraUpdate (float *cameraPosition, float x, float y, int width, int height){
    cameraPosition[0] = - (WIDTH / 2) + (x + width / 2);
    cameraPosition[1] = - (HEIGHT / 2) + (y + height / 2);

    if (cameraPosition[0] < 0)
        cameraPosition[0] = 0;
    if (cameraPosition[1] < 0)
        cameraPosition[1] = 0;
}