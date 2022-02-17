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
	int fd, flags = 0, count = 0;
	mode_t mode = 0;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	/* Keep opening files in a loop till error */
	while (1) {
		fd = open(argv[1], O_RDWR);
		if (fd == -1) {
			fprintf(stderr, "Failed to open file %s:%s,"
				"errorno=%d, count=%d\n", argv[0],
				strerror(errno), errno, count);
			exit(1);
		}
		count++;
	}
}
