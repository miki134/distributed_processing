# SOURCES=$(wildcard *.c)
# HEADERS=$(SOURCES:.c=.h)
#FLAGS=-DDEBUG -g

# SOURCES = main.c guide.c message_handler.c queue.c tourist.c util.c
# HEADERS = main.h message_handler.h queue.h util.h

SOURCES = main.c queue.c guide.c util.c message_handler.c tourist.c
HEADERS = main.h queue.h message_handler.h util.h

FLAGS=-g

all: main tags

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o main -lpthread

clear: clean

clean:
	rm main a.out

#tags: ${SOURCES} ${HEADERS}
#	ctags -R .

run: main Makefile tags
	mpirun -np 8 ./main
# mpirun -oversubscribe -np 2 ./main
# mpirun -oversubscribe -np 2 ./main
