cc := gcc
st := -Wall -g -O0 -Iinclude
src1 := src/server.c src/protocol.c src/command.c src/logger.c src/config.c src/device_state.c
src2 := src/client.c src/protocol.c src/config.c 
bu := build
ta1 := $(bu)/server
ta2 := $(bu)/client
all:$(ta1) $(ta2)
$(ta1):$(src1) | $(bu)
	$(cc) $(st) $(src1) -o $(ta1)
$(ta2):$(src2) | $(bu)
	$(cc) $(st) $(src2) -o $(ta2)
$(bu):
	mkdir -p $(bu)
run1:$(ta1)
	mkdir -p logs
	./$(ta1)
run2:$(ta2)
	./$(ta2)
clean:
	rm -rf $(bu)
.PHONY:all clean run1 run2