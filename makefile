assembler: main.o preAssembler.o utils.o firstPass.o secondPass.o labelUtils.o
	gcc -g -ansi -Wall -pedantic main.o preAssembler.o utils.o firstPass.o secondPass.o labelUtils.o -o assembler -lm

main.o: main.c main.h labelUtils.h
	gcc -c -g -ansi -Wall -pedantic main.c -o main.o

firstPass.o: firstPass.c firstPass.h labelUtils.h
	gcc -c -g -ansi -Wall -pedantic firstPass.c -o firstPass.o

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

testclean:
	rm -rf *.am *.ent *.ext *.ob
