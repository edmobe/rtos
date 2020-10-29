PHONY=window

report:
	gcc main.c -o main.o -pthread -lallegro -lallegro_image -lallegro_primitives -lallegro_dialog -lallegro_font -lallegro_ttf
	./main.o

window:
	gcc window.c mainlogic.c graphics.c -o window $(pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_dialog-5 --libs --cflags)
	report
