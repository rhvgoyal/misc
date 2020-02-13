#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret;
	mode_t mode = 0;
	char *buf;
	ssize_t written;
	char *map_addr;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	buf = memalign(4096, 4096);
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate memory at 4K alignment:%s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}

	memset(buf, 'a', 4096);

	fd = open(argv[1], O_RDWR | O_DIRECT);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Write few bytes */		
	written = write(fd, buf, 4096);
	if (written == -1)
		fprintf(stderr, "Failed to write %d bytes:%s, errorno=%d\n", 4096, strerror(errno), errno);
	close(fd);
}
