cc := gcc
st := -Wall -g -O0
src1 := src/epoll_server.c
src2 := src/tcp_client.c
bu := build
ta1 := $(bu)/epoll_server
ta2 := $(bu)/tcp_client
all:$(ta1) $(ta2)
$(ta1):$(src1) | $(bu)
	$(cc) $(st) $(src1) -o $(ta1)
$(ta2):$(src2) | $(bu)
	$(cc) $(st) $(src2) -o $(ta2)
$(bu):
	mkdir -p $(bu)
run1:$(ta1)
	./$(ta1)
run2:$(ta2)
	./$(ta2)
clean:
	rm -rf $(bu)
.PHONY:all clean run1 run2