all: MyShell

MyShell: main.o
	gcc main.o -o MyShell -lreadline

assign2final.o: main.c
	gcc -c main.c
