cc :=  /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
st := -Wall -g -O0
src := src/main.c
ta := key_monitor
all:$(ta)
$(ta):$(src)
	$(cc) $(st) $(src) -o $(ta)
deploy:
	ssh debian@192.168.7.2 "mkdir -p /home/debian/apps/day45_key_input"
	scp $(ta) debian@192.168.7.2:/home/debian/apps/day45_key_input/
clean:
	rm -rf $(ta)
.PHONY:all clean deploy
