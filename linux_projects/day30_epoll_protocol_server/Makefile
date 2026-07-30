cc := gcc
st := -Wall -g -O0 
bu:= build
sr1 := src/client.c
sr2 := src/server.c
ta1 := $(bu)/client
ta2 := $(bu)/server
all:$(ta1) $(ta2)
$(ta1): $(sr1) | $(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2): $(sr2) | $(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p build
run1:$(ta1)
	./$(ta1)
run2:$(ta2)
	./$(ta2)
clean:
	rm -rf $(bu)
.PHONY:all run1 run2 clean