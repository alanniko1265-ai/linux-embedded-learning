cc := gcc
st := -Wall -g -O0
sr := src/main.c
bu := build
ta := $(bu)/system_probe
all: $(ta)
$(ta): $(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p $(bu)
run-cpu:$(ta)
	./$(ta) cpu
run-mem:$(ta)
	./$(ta) mem
run-null:$(ta)
	./$(ta) null
run-zero:$(ta)
	./$(ta) zero
run-bad:$(ta)
	./$(ta) abc
clean:
	rm -rf $(bu)
.PHONY: all run-cpu run-mem run-null run-zero run-bad clean
	
