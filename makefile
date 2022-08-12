assembler: main2.o preAssembler.o utils.o firstPass2.o secondPass.o labelUtils.o
	gcc -g -ansi -Wall -pedantic main2.o preAssembler.o utils.o firstPass2.o secondPass.o labelUtils.o -o assembler -lm

main2.o: main2.c main.h labelUtils.h
	gcc -c -g -ansi -Wall -pedantic main2.c -o main2.o

firstPass2.o: firstPass2.c firstPass2.h labelUtils.h
	gcc -c -g -ansi -Wall -pedantic firstPass2.c -o firstPass2.o

secondPass.o: secondPass.c secondPass.h
	gcc -c -g -ansi -Wall -pedantic secondPass.c -o secondPass.o

preAssembler.o: preAssembler.c preAssembler.h utils.o
	gcc -c -g -ansi -Wall -pedantic preAssembler.c -o preAssembler.o

utils.o: utils.c utils.h structures.h
	gcc -c -g -ansi -Wall -pedantic utils.c -o utils.o

labelUtils.o: labelUtils.h
	gcc -c -g -ansi -Wall -pedantic labelUtils.c -o labelUtils.o


clean:
	rm -rf *.o assembler
