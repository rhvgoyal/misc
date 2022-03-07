#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* This creates regular file using mknod(). */
int main(int argc, char *argv[])
{
	int fd, flags = 0;
	mode_t mode = 0;

	if (argc != 2) {
		printf("Usage:%s <file-to-create>\n", argv[0]);
		exit(1);
	}

	fd = mknod(argv[1], S_IFREG | S_IRUSR | S_IWUSR, 0);
	if (fd == -1) {
		fprintf(stderr, "Failed to mknod file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}
	close(fd);
}
