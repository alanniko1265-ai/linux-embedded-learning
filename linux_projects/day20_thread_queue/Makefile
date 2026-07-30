cc := gcc
st := -Wall -g -O0 -pthread
sr := src/main.c
bu := build
ta := $(bu)/thread_queue_demo
all:$(ta)
$(ta): $(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p $(bu)
run:$(ta)
	./$(ta)
clean:
	rm -rf $(bu)
.PHONY:all run clean