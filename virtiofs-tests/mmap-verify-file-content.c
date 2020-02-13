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
	void *map_addr;
	size_t map_length = 0x4096;
	char *buf_out = "a";
	char *cur_ptr, c;

	if (argc != 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	map_addr = mmap(NULL, map_length, PROT_READ, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	/*
	 * Read whole file if any of the character is different than a, 
	 * its a corruption
	 */
	cur_ptr = map_addr;
	for (i = 0; i < 4096; i++) {
		c = cur_ptr[i];
		if (c != 'a') {
			printf("Error. Expected %c got %c\n", 'a', c);
			exit(1);
		}
		if (i % 10 == 0) {
			usleep(10);
		}
	}

	munmap(map_addr, map_length);
	close(fd);
}
