CC=gcc
CFLAGS=-c 
LDFLAGS=-lm -lrt
TARGET=universe

SOURCES=gravity.c physics.c user_input.c ascii_graphics.c
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

