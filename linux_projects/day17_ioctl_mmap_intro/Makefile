cc := gcc
st := -Wall -g -O0
sr := src/main.c
bu := build
ta := $(bu)/ioctl_mmap_intro
all: $(ta)
$(ta):$(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p build
run-winsize:$(ta)
	./$(ta) winsize
run-mmap:$(ta)
	./$(ta) mmap
run-bad:$(ta)
	./$(ta) abc
clean:
	rm -rf $(bu)
.PHONY: all run-winsize run-mmap run-bad clean
