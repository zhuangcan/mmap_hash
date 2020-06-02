
APP = mmap_write

SRC-y += mmap_write.c

CFLAGS += -O3

CFLAGS += -I../../include

MMAP_WRITE_LDLIBS += -lhash

EXTRA_LDFLAGS += -L./ $(MMAP_WRITE_LDLIBS)

all:
	gcc $(SRC-y) $(CFLAGS) $(EXTRA_LDFLAGS) -o $(APP)

clean:
	rm -rf *.a
	rm -rf $(APP)
