TARGET = eco
FLAGS = -Wall -pg
SRC = main.c creature.c
OBJ = ${SRC:.c=.o}

${TARGET}:	.c.o	${OBJ}
	gcc $(OBJ) ${FLAGS} -o $@

${OBJ}:	creature.h

.c.o:
	${CC} -pg -c ${SRC}

test: test/test_list.c creature.c creature.h
	gcc test/test_list.c creature.c -o test_list
	./test_list

clean:
	rm -f *.o test_* $(TARGET) gmon.out

.PHONY: test clean
