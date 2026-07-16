CC := gcc
AR := ar

CFLAGS := -Wall -g -O0 -Iinclude
PICFLAGS := -fPIC

BUILD_DIR := build
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin

STATIC_LIB := $(LIB_DIR)/liblog.a
SHARED_LIB := $(LIB_DIR)/liblog.so

LOG_OBJ := $(BUILD_DIR)/log.o
LOG_PIC_OBJ := $(BUILD_DIR)/log_pic.o

STATIC_APP := $(BIN_DIR)/app_static
SHARED_APP := $(BIN_DIR)/app_shared

all: static shared

static: $(STATIC_APP)

shared: $(SHARED_APP)

$(STATIC_APP): app/main.c $(STATIC_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) app/main.c -L$(LIB_DIR) -llog -o $@

$(SHARED_APP): app/main.c $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) app/main.c -L$(LIB_DIR) -llog -o $@

$(STATIC_LIB): $(LOG_OBJ) | $(LIB_DIR)
	$(AR) rcs $@ $<

$(SHARED_LIB): $(LOG_PIC_OBJ) | $(LIB_DIR)
	$(CC) -shared $< -o $@

$(LOG_OBJ): src/log.c include/log.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/log.c -o $@

$(LOG_PIC_OBJ): src/log.c include/log.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c src/log.c -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_DIR): | $(BUILD_DIR)
	mkdir -p $(LIB_DIR)

$(BIN_DIR): | $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

run-static: $(STATIC_APP)
	./$(STATIC_APP)

run-shared: $(SHARED_APP)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(SHARED_APP)

check-shared: $(SHARED_APP)
	ldd $(SHARED_APP)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all static shared run-static run-shared check-shared clean
