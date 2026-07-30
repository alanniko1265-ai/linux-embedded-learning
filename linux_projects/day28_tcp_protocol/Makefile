cc := gcc
st := -Wall -g -O0
bu := build

src1 := src/protocol_server.c
src2 := src/protocol_client.c

ta1 := $(bu)/protocol_server
ta2 := $(bu)/protocol_client

all: $(ta1) $(ta2)

$(ta1): $(src1) | $(bu)
	$(cc) $(st) $(src1) -o $(ta1)

$(ta2): $(src2) | $(bu)
	$(cc) $(st) $(src2) -o $(ta2)

$(bu):
	mkdir -p $(bu)

runse: $(ta1)
	./$(ta1)

runc: $(ta2)
	./$(ta2)

clean:
	rm -rf $(bu)

.PHONY: all clean runse runc
