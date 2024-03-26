# compiler
CC = gcc

# flags
CFLAGS =  -pedantic -Wall -Wextra -Werror -fsanitize=address -fsanitize=leak

# build target
TARGET = msh

# bin folder
FOLDER = bin

all: $(TARGET).c
	$(CC) $(CFLAGS) -o $(FOLDER)/$(TARGET) $(TARGET).c

run:
	./$(FOLDER)/$(TARGET)

clean: 
	rm $(FOLDER)/$(TARGET)
