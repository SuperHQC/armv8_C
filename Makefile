CC = gcc


all:	armv8

armv8:	armv8_c.c
	${CC} -o armv8.o armv8_c.c

armv8p: armv8_c.c
	${CC} -o armv8.o -DP armv8_c.c

run:
	./armv8.o fact.s

clean:
	rm -f *.o *.out;
