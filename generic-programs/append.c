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
	char *buf = "Hello\n";
	ssize_t written;
	off_t offset;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_APPEND);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);


	/* lseek to end of file */
/*
	offset = lseek(fd, 0, SEEK_END);
	if (offset == -1) {
		fprintf(stderr, "Failed to lseek(SEEK_END):%s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}
*/
	/* Write few bytes */		
	written = write(fd, buf, strlen(buf));
	if (written == -1)
		fprintf(stderr, "Failed to write %d bytes:%s, errorno=%d\n", strlen(buf), strerror(errno), errno);
	close(fd);
}
