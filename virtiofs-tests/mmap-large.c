#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret, i;
	mode_t mode = 0;
	void *map_addr;
	unsigned long nr_ranges;
	size_t map_length = 0x4096;
	char *endptr;
	char buf_in[16];
	char *buf_out = "Hello World";
	struct stat st_buf;
	void *cur_ptr;

	if (argc != 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	if(fstat(fd, &st_buf) == -1) {
		fprintf(stderr, "fstat failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}
	
	map_length = st_buf.st_size;
	map_addr = mmap(NULL, map_length, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);

	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	/* Write few bytes */
#define RANGE_SIZE	(2*1024*1024)

	nr_ranges = map_length/RANGE_SIZE;
	printf("map_length=%ld nr_ranges=%ld\n", map_length, nr_ranges);
	for (i = 0; i < nr_ranges; i++) {
		cur_ptr = map_addr + (unsigned)i * RANGE_SIZE;
		printf("Writing a string at address 0x%p i=%d\n", cur_ptr, i);
		memcpy(cur_ptr, buf_out, strlen(buf_out) + 1);

		/* Read few bytes */
		memcpy(buf_in, cur_ptr, 10);
		buf_in[10] = '\0';
		printf("First 10 chars from file are [%s]\n", buf_in);
	}

	printf("Will do an msync\n");

	ret = msync(map_addr, map_length, MS_SYNC);
	if (ret == -1) {
		fprintf(stderr, "msync() failed %s, errorno=%d\n",
				strerror(errno), errno);
		exit(1);	
	}
	munmap(map_addr, map_length);
	close(fd);
}
