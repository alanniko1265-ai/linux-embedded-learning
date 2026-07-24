cc :=gcc
st :=-Wall -g -O0
bu :=build
sr1 :=src/select_server.c
ta1 :=$(bu)/select_server
sr2 :=src/tcp_client.c
ta2 :=$(bu)/tcp.client
all:$(ta1) $(ta2)
$(ta1):$(sr1) |$(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) |$(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p $(bu)
runs:
	./$(ta1)
runm1:
	./$(ta2)
runm2:
	./$(ta2) 
clean:
	rm -rf $(bu)
.PHONY:all clean runm1 runm2 runs


