CC = g++
CFLAGS = -Wall -c -g

PLAYERC_CFLAGS = `pkg-config --cflags playerc`
PLAYERC_LDFLAGS = `pkg-config --libs playerc`
OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LDFLAGS = `pkg-config --libs opencv`

CFLAGS += $(PLAYERC_CFLAGS) $(OPENCV_CFLAGS)
LDFLAGS = $(PLAYERC_LDFLAGS) $(OPENCV_LDFLAGS)

simple: simple.o map.o transforms.o frontier.o
	$(CC) simple.o map.o transforms.o frontier.o -o simple $(LDFLAGS)

simple.o: simple.c laser.h
	$(CC) $(CFLAGS) simple.c

map.o: map.c map.h laser.h transforms.h frontier.h
	$(CC) $(CFLAGS) map.c

transforms.o: transforms.c transforms.h laser.h
	$(CC) $(CFLAGS) transforms.c

frontier.o: frontier.c frontier.h map.h
	$(CC) $(CFLAGS) frontier.c

clean:
	rm -f *.o *.c~ *.h~ simple
