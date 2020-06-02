

#library name
PROGNAME=libhash.a

CFLAGS += $(WERROR_FLAGS)
CFLAGS +=-O3

CFLAGS += -I../include

SRC-y += hash.c

all:
	gcc -c $(SRC-y) $(CFLAGS)
	ar rcs $(PROGNAME) *.o

clean:
	rm -rf *.o *.a
