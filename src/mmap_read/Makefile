
APP = mmap_read

SRC-y += mmap_read.c

CFLAGS += -O3

CFLAGS += -I../../include

MMAP_READ_LDLIBS += -lhash

EXTRA_LDFLAGS += -L./ $(MMAP_READ_LDLIBS)

all:
	gcc $(SRC-y) $(CFLAGS) $(EXTRA_LDFLAGS) -o $(APP)

clean:
	rm -rf *.a
	rm -rf $(APP)
