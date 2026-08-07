cc := /home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
ta := ledctl
st := -Wall -g -O0
src := src/main.c
all:$(ta)
$(ta):$(src)
	$(cc) $(st) $(src) -o $(ta)
deploy:
	scp $(ta) debian@192.168.7.2:/home/debian/apps/day43_led_control/
clean:
	rm -rf $(ta)
.PHONY:all clean deploy
