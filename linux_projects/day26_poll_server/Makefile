cc := gcc
st := -Wall -g -O0
bu := build
sr1 := src/poll_server.c
sr2 := src/tcp_client.c
ta1 := $(bu)/poll_server
ta2 := $(bu)/tcp_client
all:$(ta1) $(ta2)
$(ta1):$(sr1) |$(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) |$(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p build
runse:$(ta1)
	./$(ta1)
runc:$(ta2)
	./$(ta2)
clean:
	rm -rf $(bu)
.PHONY:runse runc clean all
