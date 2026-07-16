CC := gcc
CFLAGS := -Wall -g -O0 -Iinclude
BUILD_DIR := build
TARGET := $(BUILD_DIR)/linux_toolbox
SRC := src/main.c src/log.c src/time_utils.c

all: $(TARGET)
$(TARGET):$(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) 
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
run: $(TARGET)
	./$(TARGET) help
clean:
	rm -rf  $(BUILD_DIR)
.PHONY: all run clean
