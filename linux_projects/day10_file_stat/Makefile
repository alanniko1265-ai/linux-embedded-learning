cc := gcc
st := -Wall -g -O0
BUILD := build
ta := $(BUILD)/file_probe
SRC := src/main.c
all : $(ta)
$(ta): $(SRC) | $(BUILD)
	$(cc) $(st) $(SRC) -o $(ta)
$(BUILD):
	mkdir -p $(BUILD)
run-file:$(ta)
	./$(ta) data/input.txt
run-dir:$(ta)
	./$(ta) data
run-missing:$(ta)
	./$(ta)  data/not_exist.txt
clean:
	rm -rf $(BUILD)
.PHONY: all run-file run-dir run-missing clean