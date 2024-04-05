# compiler
CC = gcc

# flags
CFLAGS = -pedantic -Wall -Wextra -Werror -fsanitize=address -fsanitize=leak

# build target
SRC_FOLDER = src
UTILS = utils
TEST_FOLDER = tests
TARGET = msh

# bin folder
BIN_FOLDER = bin

# test bin folder
TEST_BIN_FOLDER = bin/tests

# list of source files
SOURCES = $(wildcard $(SRC_FOLDER)/*.c)

# list of test source files
TEST_SOURCES = $(wildcard $(TEST_FOLDER)/*.c)

# list of object files
OBJECTS = $(SOURCES:$(SRC_FOLDER)/%.c=$(BIN_FOLDER)/%.o)

# list of test object files
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_FOLDER)/%.c=$(TEST_BIN_FOLDER)/%.o)

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

# test target
test: $(TEST_BIN_FOLDER)/$(TARGET)_test

# link test object files into test executable
$(TEST_BIN_FOLDER)/$(TARGET)_test: $(TEST_OBJECTS) $(filter-out $(BIN_FOLDER)/$(TARGET).o, $(OBJECTS))
	$(CC) $(CFLAGS) -o $@ $^

# compile each test source file into object files
$(TEST_BIN_FOLDER)/%.o: $(TEST_FOLDER)/%.c $(SRC_FOLDER)/$(UTILS).h | $(TEST_BIN_FOLDER)
	$(CC) $(CFLAGS) -c -o $@ $<

# create test bin folder if not exists
$(TEST_BIN_FOLDER):
	mkdir -p $(TEST_BIN_FOLDER)

run: all
	./$(BIN_FOLDER)/$(TARGET)

test_run: test
	./$(TEST_BIN_FOLDER)/$(TARGET)_test

clean:
	rm -rf $(BIN_FOLDER) $(TEST_BIN_FOLDER)

.PHONY: all run clean test_run test
