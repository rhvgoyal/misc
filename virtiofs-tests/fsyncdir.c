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

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	/* Directory needs to be opened O_RDONLY */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		fprintf(stderr, "Failed to open dir %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);


	ret = fsync(fd);
	if (ret == -1)
		fprintf(stderr, "Failed to fsync() dir %s:%s, errorno=%d\n",
				argv[1], strerror(errno), errno);
	close(fd);
}
