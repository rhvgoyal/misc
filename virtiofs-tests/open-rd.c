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

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	memset(buf, 0, buflen);
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[1], strerror(errno), errno);


	/* REad few bytes */		
	ret = read(fd, buf, buflen - 1);
	if (ret == -1) {
		fprintf(stderr, "Failed to read %d bytes:%s, errorno=%d\n", strlen(buf), strerror(errno), errno);
	}
	close(fd);

	printf("Read [%s] from file %s\n", buf, argv[1]);
}
