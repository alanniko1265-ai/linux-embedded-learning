cc := gcc
st := -Wall -g -O0 
bu := build
ta := build/proc_runner
sr := src/main.c
all:$(ta)
$(ta):$(sr)|$(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p $(bu)
run-ls:$(ta)
	./$(ta) /bin/ls data
run-echo:$(ta)
	./$(ta) /bin/echo hello linux
run-fail-cmd:$(ta)
	./$(ta) /bin/not_exist
run-fail-arg:$(ta)
	./$(ta) /bin/ls not_exist_dir
clean:
	rm -rf $(bu)
.PHONY:all run-ls run-echo run-fail-cmd run-fail-arg