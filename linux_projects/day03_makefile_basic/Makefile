CC := gcc
CFLAGS := -Wall -g -O0 -Iinclude

BUILD_DIR := build
TARGET := $(BUILD_DIR)/calc_tool

SRC := src/main.c src/calc.c
OBJ := $(SRC:src/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(OBJ) -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

.PHONY: all run clean rebuild
