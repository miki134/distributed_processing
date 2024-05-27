# SOURCES=$(wildcard *.c)
# HEADERS=$(SOURCES:.c=.h)
#FLAGS=-DDEBUG -g

SOURCES = main.c guide.c message_handler.c queue.c tourist.c util.c
HEADERS = main.h message_handler.h queue.h util.h

FLAGS=-g

all: main tags

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o main

clear: clean

clean:
	rm main a.out

#tags: ${SOURCES} ${HEADERS}
#	ctags -R .

run: main Makefile tags
	mpirun -oversubscribe -np 8 ./main
