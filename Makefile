CC=gcc
CFLAGS= -g -c `pkg-config --cflags gtk+-2.0` 
LDFLAGS=-lm -lrt `pkg-config --libs gtk+-2.0` 
TARGET=universe

SOURCES=gravity.c physics.c user_input.c gui.c object.c
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

