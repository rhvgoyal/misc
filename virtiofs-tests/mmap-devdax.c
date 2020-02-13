#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE	4096
int main(int argc, char *argv[])
{
	int fd, i;
	void *map_addr;
	size_t map_length;
	char buf_in[PAGE_SIZE];
	struct stat st_buf;
	void *cur_ptr;
	unsigned long nr_pages;

	if (argc != 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	if (fstat(fd, &st_buf) == -1) {
		fprintf(stderr, "Failed to stat file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	map_length = st_buf.st_size;
	/* devdax is char device and stat() returns length as 0. Hard code
	 * length for now
	 */
	map_length = 2145386496;
	printf("Size of device is %ld\n", st_buf.st_size);

	map_addr = mmap(NULL, map_length, PROT_READ, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	/* Read whole disk in 4K page size */
	nr_pages = map_length/PAGE_SIZE;

	cur_ptr = map_addr;
	for (i = 0; i < nr_pages; i++) {
		memcpy(buf_in, cur_ptr, PAGE_SIZE);
		cur_ptr += PAGE_SIZE;
	}

	munmap(map_addr, map_length);
	close(fd);
}
