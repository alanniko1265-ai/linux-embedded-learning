cc := gcc
st := -Wall -g -O0 
bu := build
src := src/main.c
ta := $(bu)/nonblock_demo
all :$(ta)
$(ta):$(src) |$(bu)
	$(cc) $(st) $(src) -o $(ta)
$(bu):
	mkdir -p build
run:$(ta)
	./$(ta)
clean:
	rm -rf $(bu)
.PHONY: all run clean
