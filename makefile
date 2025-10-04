tuifd.o:
	gcc -o ./build/tuifd -g ./src/* -I./include -lncurses -lpthread
test.o:
	gcc -o ./build/test -g ./test.c -I./include -lncurses -lpthread
