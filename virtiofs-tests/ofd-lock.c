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
	struct flock flock;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
	}

	/* Take ofd lock */		
	memset(&flock, 0, sizeof(flock));
	flock.l_type = F_WRLCK;
	flock.l_whence = SEEK_SET;
	flock.l_start = 0;
	flock.l_len = 0; /* Lock whole file */

	ret = fcntl(fd, F_OFD_SETLK, &flock);
	if (ret == -1) {
		fprintf(stderr, "Failed to lock file:%s, errorno=%d\n",
			strerror(errno), errno);
	}
	printf("Press a key to exit.\n");
	scanf("d");
	close(fd);
}
