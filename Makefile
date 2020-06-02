
all:
	make -C hash
	@cp hash/*.a src/mmap_read
	@cp hash/*.a src/mmap_write
	make -C src/mmap_read
	make -C src/mmap_write

clean:
	make -C hash clean
	make -C src/mmap_read clean
	make -C src/mmap_write clean
