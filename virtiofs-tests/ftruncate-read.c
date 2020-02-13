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
	char buf[16];
	ssize_t ret;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT, 0644);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);


	/* ftruncate */
	ftruncate(fd, 0);

	/* Read few bytes */		
	memset(buf, 0, 16);
	ret = read(fd, buf, 15);
	if (ret == -1) {
		fprintf(stderr, "Failed to read %d bytes:%s, errorno=%d\n", 15, strerror(errno), errno);
	} else {
		printf("Read from truncated region returned=%d\n", ret);
	}

	close(fd);
}
