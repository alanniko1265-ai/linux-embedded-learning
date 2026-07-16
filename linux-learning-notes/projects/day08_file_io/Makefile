CC := gcc
CFLAGS := -Wall -g -O0
BUILD_DIR=build
MY_CAT :=$(BUILD_DIR)/my_cat
MY_CP :=$(BUILD_DIR)/my_cp
all: $(MY_CAT) $(MY_CP)
$(MY_CAT):src/read_file.c |$(BUILD_DIR)
	$(CC) $(CFLAGS) src/read_file.c -o $(MY_CAT)
$(MY_CP): src/my_cp.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/my_cp.c -o $(MY_CP)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
run-cat:$(MY_CAT)
	./$(MY_CAT) data/input.txt
run-cp: $(MY_CP) $(MY_CAT)
	./$(MY_CP) data/input.txt data/output_make.txt
	./$(MY_CAT) data/output_make.txt
clean:
	rm -rf $(BUILD_DIR)
.PHONY:all run-cat run-cp clean