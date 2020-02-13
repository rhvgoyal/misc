#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0;
	mode_t mode = 0;
	char buf[10];
	int buflen = 10;
	ssize_t ret;
	unsigned hole_size, hole_offset;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	hole_size = hole_offset = 4096;
	memset(buf, 0, buflen);
	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[1], strerror(errno), errno);
		exit(1);
	}

	/* Punch hole at offset 4K */
	printf("Punching hole of size 4K at offset 4K\n");
	ret = fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
			hole_offset, hole_size);
	if (ret == -1) {
		fprintf(stderr, "Failed to punch hole in file %s:%s, errorno=%d\n", argv[1], strerror(errno), errno);
		exit(1);
	}

	/* REad few bytes from hole*/
	ret = pread(fd, buf, buflen - 1, hole_offset);
	if (ret == -1) {
		fprintf(stderr, "Failed to read %d bytes:%s, errorno=%d\n", strlen(buf), strerror(errno), errno);
		exit(1);
	}
	close(fd);

	printf("Read [%s] from file %s\n", buf, argv[1]);
}
