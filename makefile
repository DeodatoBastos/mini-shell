# compiler
CC = gcc

# flags
CFLAGS = -Wall

# build target
TARGET = msh

all: $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

run:
	./$(TARGET)

clean: 
	rm $(TARGET)
