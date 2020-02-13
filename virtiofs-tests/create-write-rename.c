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
	int fd, flags = 0, ret;
	mode_t mode = 0;
	char *buf = "Hello";
	ssize_t written;
	char *filename;

	if (argc != 2) {
		printf("Usage:%s <file-to-create>\n", argv[0]);
		exit(1);
	}

	ret = asprintf(&filename, "%s.tmp", argv[1]);
	if (ret == -1) {
		fprintf(stderr, "Failed to use asprintf %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	fd = open(filename, O_RDWR | O_CREAT);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", filename, strerror(errno), errno);
		exit(1);
	}

	/* Write few bytes */		
	written = write(fd, buf, strlen(buf));
	if (written == -1) {
		fprintf(stderr, "Failed to write %d bytes:%s, errorno=%d\n", strlen(buf), strerror(errno), errno);
		exit(1);
	}

	close(fd);
	/* Rename */
	ret = rename(filename, argv[1]);
	if (ret == -1) {
		fprintf(stderr, "Failed to rename %s to %s:%s, errorno=%d\n", filename, argv[1], strerror(errno), errno);
	}
}
