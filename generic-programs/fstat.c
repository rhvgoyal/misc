#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	int ret, flags;
	struct stat statbuf;

	if (argc != 2) {
		printf("Usage:%s <file-to-stat>\n", argv[0]);
		exit(1);
	}

	flags = AT_NO_AUTOMOUNT;
	ret = fstatat(AT_FDCWD, argv[1], &statbuf, flags);
	if (ret == -1) {
		fprintf(stderr, "Failed to stat file %s:%s, errorno=%d\n",
			argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Print few elements */
	printf("st_dev=0x%x st_ino=%lu\n", statbuf.st_dev, statbuf.st_ino);
}
