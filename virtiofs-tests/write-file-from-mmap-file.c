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
	int fd, fd2, flags = 0, ret;
	mode_t mode = 0;
	void *map_addr;
	size_t map_length = 0x4096;
	char *endptr;
	char buf_in[16];
	char *buf_out = "Hello World";
	struct stat st_buf;
	void *cur_ptr;

	if (argc != 3 ) {
		printf("Usage:%s <file-to-mmap> <file-to-write-to>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[1], strerror(errno), errno);
		exit(1);
	}

	map_addr = mmap(NULL, map_length, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno),
			errno);
		exit(1);
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	/* Open file which is being written to */
	fd2 = open(argv[2], O_RDWR | O_APPEND);
	if (fd2 == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[2], strerror(errno), errno);
		exit(1);
	}

	/* Write few bytes to mmaped file */
	cur_ptr = map_addr;
	printf("Writing a string at address 0x%p\n", cur_ptr);
	memcpy(cur_ptr, buf_out, strlen(buf_out) + 1);

	printf("Writing to file %s from mmaped file %s\n", argv[2], argv[1]);
	/* Write to file with source being from mmaped file */
	ret = write(fd2, map_addr, strlen(buf_out) + 1);
	if (ret == -1) {
		fprintf(stderr, "Failed to write to file %s:%s, errorno=%d\n",
			argv[2], strerror(errno), errno);
	}

	munmap(map_addr, map_length);
	close(fd);
	close(fd2);
}
