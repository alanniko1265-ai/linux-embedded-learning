cc := gcc
st := -Wall -g -O0
sr1 := src/tcp_client.c
sr2 := src/tcp_server.c
bu := build
ta1 := $(bu)/tcp_client
ta2 := $(bu)/tcp_server
all:$(ta1) $(ta2)
$(ta1):$(sr1) |$(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) |$(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p $(bu)
run1:$(ta2)
	./$(ta2)
run2:$(ta1)
	./$(ta1) hello tcp
run3:$(ta1)
	./$(ta1) status
run4:$(ta1)
	./$(ta1) set led on
clean:
	rm -rf build
.PHONY:all run1 run2 run3 run4 clean