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
	char *buf = "Hello";
	ssize_t written;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);


	/* Write few bytes */		
	written = write(fd, buf, strlen(buf));
	if (written == -1)
		fprintf(stderr, "Failed to write %d bytes:%s, errorno=%d\n", strlen(buf), strerror(errno), errno);
	close(fd);
}
