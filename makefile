assembler: main.o preAssembler.o
	gcc -g -ansi -Wall -pedantic main.o preAssembler.o -o assembler -lm
main.o: main.c main.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o
preAssembler.o: preAssembler.c preAssembler.h
	gcc -c -ansi -Wall -pedantic preAssembler.c -o preAssembler.o
