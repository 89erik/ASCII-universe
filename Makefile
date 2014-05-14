CC=gcc
CFLAGS= -g -c -Wall `pkg-config --cflags gtk+-2.0` 
LDFLAGS=-lm -lrt `pkg-config --libs gtk+-2.0` 
TARGET=gravsim

SOURCES= physics.c user_input.c gui.c object.c vector.c
OBJECTS=$(SOURCES:.c=.o)

.PHONY : all
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@




.PHONY : clean
clean :
	rm -rf *.o

