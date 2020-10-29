#include "window.h"

/*=========================== MAIN ===========================*/
int main()
{
/*=========================== TABLE LOGIC ====================*/
    /* Initialize alien array, report, and random seed */
    initialize();
    srand(time(NULL));

    // /* Add processes */
    // append(1, 6, 0);
    // append(2, 9, 0);
    // append(6, 18, 0);

    // /* Modify positions to test exit */
    // /*
    // alienArray.aliens[0].posX = 17;
    // alienArray.aliens[0].posY = 1;
    // alienArray.aliens[1].posX = 16;
    // alienArray.aliens[1].posY = 1;
    // */

    // printf("Created array is: ");
    // printAlienArray();
    // printf("\n");

    // printMaze();

    // int iterationCounter = 0;
    // while (!finished)
    // {
    //     rm(iterationCounter);
    //     sleep(1);
    //     iterationCounter++;
    // }

    // printf("================== REPORT ==================\n");

    // // Print processes
    // for (int i = 0; i < alienArray.length; i++)
    // {
    //     printf("Process %d initialized in cycle %d\n",
    //            alienArray.aliens[i].id, alienArray.aliens[i].appendedIteration);
    // }

    // // Print report
    // for (int i = 0; i < report.iterations; i++)
    // {
    //     if (report.log[i] == -1)
    //         printf("[%d - %d]: ?\n", i, i + 1);
    //     else
    //         printf("[%d - %d]: Process %d\n", i, i + 1, report.log[i]);
    // }

/*=========================== GUI =============================*/
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");
    must_init(al_init_font_addon(), "font");
    must_init(al_init_ttf_addon(), "ttfont");
    // al_init_native_dialog_addon()
    must_init(al_init_image_addon(), "image");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init();

    ALLEGRO_FONT* font = al_load_ttf_font("fonts/_decterm.ttf", 14, 0);
    must_init(font, "font");

    ALLEGRO_FONT* titlefont = al_load_ttf_font("fonts/_decterm.ttf", 36, 0);
    must_init(titlefont, "title font");

    ALLEGRO_FONT* secondfont = al_load_ttf_font("fonts/_decterm.ttf", 22, 0);
    must_init(secondfont, "second font");

    must_init(al_init_primitives_addon(), "primitives");

    actual_buttons = al_get_mouse_num_buttons();
    if (actual_buttons > NUM_BUTTONS)
        actual_buttons = NUM_BUTTONS;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source()); 
    // al_hide_mouse_cursor(disp);

    /*======================= ELEMENTS INIT ==========================*/
    button_init((DISP_W-DISP_H)/6,(DISP_H/24)*5+10,(DISP_W-DISP_H)/6, (DISP_H/16), 
        startlogic, "Start", al_map_rgb(0, 255, 0));
    button_init((DISP_W-DISP_H)/6,(DISP_H/24)*22+10,(DISP_W-DISP_H)/6,(DISP_H/16), 
        add_alien, "Add", al_map_rgb(255, 255, 255));
    combo_init(3,(DISP_H/24)*3+10,(DISP_W-DISP_H)/2-10,(DISP_H/24));
    keyboard_init();
    textarea_init();
    sprites_init();
    datainput_init(font);

    int i;
    done = false;
    redraw = true;
    mode = AUTO;
    ALLEGRO_EVENT event;
    
    al_start_timer(timer);
    
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                ambientmode();
                datainput_update();

                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                redraw = true;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        keyboard_update(&event);
        click_update(&event);

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            // Title
            al_draw_text(titlefont, al_map_rgb(0, 200, 50), 
                ((DISP_W-DISP_H)/2-al_get_text_width(titlefont, "RTOS"))/2, DISP_H/24, 0, "RTOS");
            // Title mark
            al_draw_rectangle(3,3,(DISP_W-DISP_H)/2-10, (DISP_H/24)*3, al_map_rgb(100,100,100), 3);
            al_draw_rectangle(20,20,(DISP_W-DISP_H)/2-27, (DISP_H/24)*3-17, al_map_rgb(100,100,100), 10);
            // Separators
            al_draw_line((DISP_W-DISP_H)/2, 0, (DISP_W-DISP_H)/2, DISP_H, al_map_rgb_f(1, 1, 1), 1);
            al_draw_line((DISP_W-DISP_H)/2-4, 0, (DISP_W-DISP_H)/2-4, DISP_H, al_map_rgb_f(1, 1, 1), 1);
            al_draw_line(DISP_W-(DISP_W-DISP_H)/2, 0, DISP_W-(DISP_W-DISP_H)/2, DISP_H, al_map_rgb_f(1, 1, 1), 1);
            al_draw_line(DISP_W-(DISP_W-DISP_H)/2+4, 0, DISP_W-(DISP_W-DISP_H)/2+4, DISP_H, al_map_rgb_f(1, 1, 1), 1);
            // Combobox
            combo_draw(font);
            // Buttons
            button_draw(font);
            // Text area (alien list)
            textarea_draw(font);
            // Data input
            datainput_draw(font);
            // Maze
            maze_draw();
            // Energy inidicator
            al_draw_text(secondfont, al_map_rgb(60, 200, 50), 
                DISP_W-(DISP_W-DISP_H)/2 + 10, DISP_H/24, 0, "Energy Level:");
            // Moving alien
            al_draw_rectangle(DISP_W-(DISP_W-DISP_H)/2+10,DISP_H/2-15,DISP_W-15, DISP_H-10, al_map_rgb(255,255,255), 3);
            al_draw_text(secondfont, al_map_rgb(2, 255, 188), 
                DISP_W-(DISP_W-DISP_H)/4, DISP_H/2, ALLEGRO_ALIGN_CENTER, "Now moving");
            // al_draw_bitmap(sprites.alien_down, 300, 300, 0);
            // al_draw_bitmap(sprites.alien_up, 300, 330, 0);
            // al_draw_bitmap(sprites.alien_left, 300, 360, 0);
            // al_draw_bitmap(sprites.alien_right, 300, 390, 0);

            // for(int i = 1; i < DISP_H+1; i=i+20)
            // {
            //     al_draw_filled_triangle(275, i, 258, i+10, 275, i+20, 
            //         al_map_rgb_f(1,1,1));
            //     al_draw_filled_triangle(1005, i, 1022, i+10, 1005, i+20, 
            //         al_map_rgb_f(1,1,1));
            // }
            // al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", x, y);
            // al_draw_filled_rectangle(x, y, x + 10, y + 10, al_map_rgb(255, 0, 0));

            al_flip_display();

            redraw = false;
        }
    }

    sprites_deinit();
    al_destroy_font(font);
    al_destroy_font(titlefont);
    al_destroy_font(secondfont);
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}