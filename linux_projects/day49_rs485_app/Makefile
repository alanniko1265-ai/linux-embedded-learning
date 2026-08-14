cc := /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
st := -Wall -Wextra -Wpedantic -g -O0 -Iinclude
src := src/main.c src/rs485.c
bu := build
ta :=$(bu)/rs485_tool
all:$(ta)
$(ta):$(src) | $(bu)
	$(cc) $(st) $(src) -o $(ta)
$(bu):
	mkdir -p $(bu)
clean:
	rm -rf $(bu)
.PHONY:all clean