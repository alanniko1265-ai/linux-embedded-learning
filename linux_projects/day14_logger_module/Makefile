cc := gcc
st := -Wall -g -O0 -Iinclude
bu := build
sr := src/main.c src/logger.c
ta := $(bu)/logger_demo
all: $(ta)
$(ta):$(sr) |$(bu)
	$(cc) $(st) $(sr) -o $(ta)
$(bu):
	mkdir -p build
run:$(ta)
	./$(ta)
show-log:
	cat logs/app.log
clean:
	rm -rf $(bu) logs/app.log
.PHONY:all run show-log clean