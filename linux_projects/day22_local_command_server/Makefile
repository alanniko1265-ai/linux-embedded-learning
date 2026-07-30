cc := gcc
st := -Wall -g -O0
sr1 := src/client.c
sr2 := src/server.c
bu := build
ta1 := $(bu)/client
ta2 := $(bu)/server
all:$(ta1) $(ta2)
$(ta1):$(sr1) |$(bu)
	$(cc) $(st) $(sr1) -o $(ta1)
$(ta2):$(sr2) |$(bu)
	$(cc) $(st) $(sr2) -o $(ta2)
$(bu):
	mkdir -p build
run:$(ta2)
	./$(ta2)
send1:$(ta1)
	./$(ta1) status
send2:$(ta1)
	./$(ta1) set led on
send3:$(ta1)
	./$(ta1) reboot device
clean:
	rm -rf $(bu)
.PHONY:clean send3 send2 send1 run
 