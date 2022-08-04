assembler: main.o preAssembler.o utils.o firstPass.o
	gcc -g -ansi -Wall -pedantic main.o preAssembler.o utils.o firstPass.o -o assembler -lm
main.o: main.c main.h 
	gcc -c -g -ansi -Wall -pedantic main.c -o main.o
firstPass.o: firstPass.c firstPass.h
	gcc -c -g -ansi -Wall -pedantic firstPass.c -o firstPass.o
preAssembler.o: preAssembler.c preAssembler.h utils.o
	gcc -c -g -ansi -Wall -pedantic preAssembler.c -o preAssembler.o
utils.o: utils.c utils.h structures.h
	gcc -c -g -ansi -Wall -pedantic utils.c -o utils.o


clean:
	rm -rf *.o assembler

