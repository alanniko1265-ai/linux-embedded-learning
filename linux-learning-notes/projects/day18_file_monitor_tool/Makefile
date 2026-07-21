cc := gcc
st := -Wall -g -O0
sr := src/main.c
bu := build
ta := $(bu)/file_monitor_tool
all:$(ta)
$(ta):$(sr) | $(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p $(bu)
run:$(ta)
	./$(ta) config/monitor.conf
show_log:
	cat logs/monitor.log
append:
	echo "new line" >> data/watch.txt
clean:
	rm -rf $(bu) logs/monitor.log
.PHONY:all run show_log append clean
