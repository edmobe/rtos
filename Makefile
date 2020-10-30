PHONY: run

APP=window
SRCS=$(APP).c graphics.c mainlogic.c report.c
CC=gcc
CFLAGS=`pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_dialog-5 allegro_ttf-5 --libs --cflags`

run: $(APP)
		./$(APP)

$(APP): $(SRCS)
		$(CC) $(SRCS) -o $(APP) $(CFLAGS) -pthread