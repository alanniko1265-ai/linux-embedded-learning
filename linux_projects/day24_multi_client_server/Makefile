cc := gcc
st :=-Wall -g -O0 -pthread
bu :=build
sr1 :=src/tcp_client.c
sr2 :=src/tcp_server.c
ta2 :=$(bu)/tcp_server
ta1 :=$(bu)/tcp_client
all:$(ta1) $(ta2)
$(ta1):$(sr1) |$(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) |$(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p build
run-se:$(ta2)
	./$(ta2)
run-m1:$(ta1)
	./$(ta1) hello tcp
run-m2:$(ta1)
	./$(ta1) status
run-m3:$(ta1)
	./$(ta1) set led on
clean:
	rm -rf $(bu)
.PHONY:clean run-m1 run-m2 run-m3 all run-se