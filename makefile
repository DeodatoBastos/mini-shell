# compiler
CC = gcc

# flags
CFLAGS = -pedantic -Wall -Wextra -Werror -fsanitize=address -fsanitize=leak

# build target
SRC_FOLDER = src
UTILS = utils
TARGET = msh

# bin folder
BIN_FOLDER = bin

# list of source files
SOURCES = $(wildcard $(SRC_FOLDER)/*.c)

# list of object files
OBJECTS = $(SOURCES:$(SRC_FOLDER)/%.c=$(BIN_FOLDER)/%.o)

# main target
all: $(BIN_FOLDER)/$(TARGET)

# link object files into executable
$(BIN_FOLDER)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# compile each source file into object files
$(BIN_FOLDER)/%.o: $(SRC_FOLDER)/%.c $(SRC_FOLDER)/$(UTILS).h | $(BIN_FOLDER)
	$(CC) $(CFLAGS) -c -o $@ $<

# create bin folder if not exists
$(BIN_FOLDER):
	mkdir -p $(BIN_FOLDER)

run: all
	./$(BIN_FOLDER)/$(TARGET)

clean:
	rm -rf $(BIN_FOLDER)

.PHONY: all run clean
