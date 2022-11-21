TARGET = eco
FLAGS = -Wall -fopenmp
SRC = main.c creature.c
OBJ = ${FILES:.c=.o}

${TARGET}:	.c.o	${OBJ}
	gcc main.c creature.c -Wall ${FLAGS} -o $@

${OBJ}:	creature.h

.c.o:
	${CC} -c ${SRC}

test: test/test_list.c creature.c creature.h
	gcc test/test_list.c creature.c -o test_list
	./test_list

clean:
	rm -f *.o

.PHONY: test clean
