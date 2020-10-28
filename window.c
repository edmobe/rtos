// #include "mainlogic.h"
#include "window.h"

/*=========================== MAIN ===========================*/
int main()
{
/*=========================== TABLE LOGIC ====================*/
    /* Initialize alien array, report, and random seed */
    // initialize();
    // srand(time(NULL));

    // /* Add processes */
    // append(2, 6, 0);
    // append(4, 9, 0);

    // printf("Created array is: ");
    // printAlienArray();
    // printf("\n");

    // printMaze();

    // for (int i = 0; i < 26; i++)
    // {
    //     if (i == 9)
    //     {
    //         append(4, 9, i);
    //         append(1, 9, i);
    //     }
    //     edf(i);
    //     sleep(1);
    // }
    // printf("================== REPORT ==================\n");

    // // Print processes
    // for (int i = 0; i < alienArray.length; i++)
    // {
    //     printf("Process %d initialized in cycle %d\n",
    //         alienArray.aliens[i].id, alienArray.aliens[i].appendedIteration);
    // }

    // // Print report
    // for (int i = 0; i < report.iterations; i++)
    // {
    //     if (report.log[i] == -1)
    //         printf("[%d - %d]: ?\n", i, i + 1);
    //     else
    //         printf("[%d - %d]: Process %d\n", i, i + 1, report.log[i]);
    // }

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

    int i;
    in = true;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_primitives_addon(), "primitives");

    actual_buttons = al_get_mouse_num_buttons();
    if (actual_buttons > NUM_BUTTONS)
        actual_buttons = NUM_BUTTONS;

    button_init(100,100,100,50, sayhello, al_map_rgb(255, 255, 255));

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source()); 
    // al_hide_mouse_cursor(disp);

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    float x, y;
    x = 100;
    y = 100;

    #define KEY_SEEN     1
    #define KEY_RELEASED 2

    float dx, dy;
    dx = 0;
    dy = 0;

    // al_grab_mouse(disp);

    unsigned char key[ALLEGRO_KEY_MAX];
    
    memset(key, 0, sizeof(key));

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;
            
            case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                in = true;
                break;

            case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
                in = false;
                break;

                // x += dx;
                // y += dy;

                // if(x < 0)
                // {
                //     x *= -1;
                //     dx *= -1;
                // }
                // if(x > 640)
                // {
                //     x -= (x - 640) * 2;
                //     dx *= -1;
                // }
                // if(y < 0)
                // {
                //     y *= -1;
                //     dy *= -1;
                // }
                // if(y > 480)
                // {
                //     y -= (y - 480) * 2;
                //     dy *= -1;
                // }

                // dx *= 0.9;
                // dy *= 0.9;

                // for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                //     key[i] &= KEY_SEEN;

                // redraw = true;
                // break;

            // case ALLEGRO_EVENT_MOUSE_AXES:
            //     dx += event.mouse.dx * 0.1;
            //     dy += event.mouse.dy * 0.1;
            //     al_set_mouse_xy(disp, 320, 240);
            //     break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        click_update(&event);

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", x, y);
            al_draw_filled_rectangle(x, y, x + 10, y + 10, al_map_rgb(255, 0, 0));
            button_draw();

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}