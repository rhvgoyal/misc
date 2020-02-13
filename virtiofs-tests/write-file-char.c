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
	char *buf_out = "a";

	if (argc != 2) {
		printf("Usage:%s <file-to-write>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	for (i = 0; i < 8192; i++) {
		write(fd, buf_out, 1);
	}

	close(fd);
}
