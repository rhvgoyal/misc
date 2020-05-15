#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

/* A simple test for futimens() */
int main(int argc, char *argv[])
{
	int fd, ret;
	struct timespec times[2];

	if (argc != 2) {
		printf("Usage:%s <file-path>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s\n", argv[1],
				strerror(errno));
		exit(1);
	}


	times[0].tv_nsec = UTIME_NOW;
	times[1].tv_nsec = UTIME_NOW;

	ret = futimens(fd, times);
	if (ret == -1) {
		fprintf(stderr, "futimens() failed:%s\n", strerror(errno));
	}
	close(fd);
}
