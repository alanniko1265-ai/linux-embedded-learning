cc := gcc
st := -Wall -g -O0
bu := build
pi :=$(bu)/pipe_demo
re :=$(bu)/fifo_reader
wr :=$(bu)/fifo_writer
all:$(pi) $(re) $(wr)
$(pi):src/pipe_demo.c |$(bu)
	$(cc) $(st) src/pipe_demo.c -o $(pi)
$(re):src/fifo_reader.c |$(bu)
	$(cc) $(st) src/fifo_reader.c -o $(re)
$(wr):src/fifo_writer.c |$(bu)
	$(cc) $(st) src/fifo_writer.c -o $(wr)
$(bu):
	mkdir -p build
runp:$(pi)
	./$(pi)
runr:$(re)
	./$(re)
runw:$(wr)
	./$(wr)
clean:
	rm -rf build
.PHONY:all runp runr runw clean
	