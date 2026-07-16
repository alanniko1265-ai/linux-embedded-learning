cc := gcc
st := -Wall -g -O0
bu := build
ta := $(bu)/dir_scan
sr := src/main.c
all:$(ta)
$(ta):$(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p $(bu)
run:$(ta)
	./$(ta) data
run-m:$(ta)
	./$(ta) not_exist_dir
clean:
	rm -rf $(bu)
.PHONY:all run run-m clean
