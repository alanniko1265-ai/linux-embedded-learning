hcc:= gcc
acc := arm-linux-gnueabihf-gcc
cst := -Wall -g -O0
src := hello_board.c
ta1 := hello_x86
ta2 := hello_arm
ta3 := hello_arm_static
all:$(ta1) $(ta2) $(ta3)
$(ta1): $(src)
	$(hcc) $(cst) $(src) -o $(ta1)
$(ta2): $(src)
	$(acc) $(cst) $(src) -o $(ta2)
$(ta3): $(src)
	$(acc) $(cst) -static $(src) -o $(ta3)
clean:
	rm -f $(ta1) $(ta2) $(ta3)
.PHONY:all clean