tuifd.o:
	gcc -o ./build/tuifd -g ./src/*.c -I./include -lncurses -lpthread
test.o:
	gcc -o ./build/test -g ./test.c -I./include -lncurses -lpthread
