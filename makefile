assembler: main.o preAssembler.o utils.o
	gcc -g -ansi -Wall -pedantic main.o preAssembler.o utils.o -o assembler -lm
main.o: main.c 
	gcc -c -ansi -Wall -pedantic main.c -o main.o
preAssembler.o: preAssembler.c preAssembler.h utils.o
	gcc -c -ansi -Wall -pedantic preAssembler.c -o preAssembler.o
utils.o: utils.c utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o
