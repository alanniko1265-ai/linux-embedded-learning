cc := gcc
st := -Wall -g -O0 -Iinclude
BUILD := build
ta :=$(BUILD)/safe_cp
SRC :=src/main.c src/file_utils.c
all :$(ta)
$(ta):$(SRC)|$(BUILD)
	$(cc) $(st) $(SRC) -o $(ta)
$(BUILD):
	mkdir -p $(BUILD)
run: $(ta)
	./$(ta) data/input.txt data/output.txt
check:run
	cmp data/input.txt data/output.txt
clean:
	rm -rf $(BUILD) data/output.txt
.PHONY:all run check clean
