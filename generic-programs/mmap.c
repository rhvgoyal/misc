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
	int fd, flags = 0;
	mode_t mode = 0;
	void *map_addr;
	unsigned long requested_addr;
	size_t map_length = 0x200000;
	char *endptr;

	if (argc < 2  || argc > 3) {
		printf("Usage:%s <file-to-mmap> [<addr-to-map-at-in-hex>]\n", argv[0]);
		exit(1);
	}

	if (argc == 3) {
		errno = 0;
		requested_addr = strtoul(argv[2], &endptr, 16);
		if (errno) {
			fprintf(stderr, "strtoul(%s) failed. %s\n", argv[2], strerror(errno));
			exit(1);
		}
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);

	if (argc == 3)
		map_addr = mmap((void*)requested_addr, map_length, PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
	else
		map_addr = mmap(NULL, map_length, PROT_WRITE, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	munmap(map_addr, map_length);
	close(fd);
}
