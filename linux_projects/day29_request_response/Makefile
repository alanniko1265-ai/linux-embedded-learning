cc := gcc
st := -Wall -g -O0
bu := build
sr1 := src/server.c
sr2 := src/client.c
ta1 := $(bu)/server
ta2 := $(bu)/client
all:$(ta1) $(ta2)
$(ta1):$(sr1) | $(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) | $(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p $(bu)
run1:$(ta1)
	./$(ta1)
run2:$(ta2)
	./$(ta2)
clean:
	rm -rf $(bu)
.PHONY:clean run1 run2 all