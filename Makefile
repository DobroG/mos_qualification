CC=gcc
CFLAGS=-I.
DEPS = flip.h img.h
OBJ = flip.o img.o 

%.o: %.c $(DEPS)
	$(CC) -Wall -std=c99 -c -o $@ $< $(CFLAGS)

flip: $(OBJ)
	gcc -Wall -std=c99 -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o flip

