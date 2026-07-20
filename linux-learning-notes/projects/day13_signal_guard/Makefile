cc := gcc
st := -Wall -g -O0
bu := build
ta := $(bu)/signal_guard
sr := src/main.c
all:$(ta)
$(ta):$(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p build
run:$(ta)
	./$(ta)
show-log:
	cat signal_guard.log
clean:
	rm -rf $(bu)  signal_guard.log
.PHONY:all run show-log clean
