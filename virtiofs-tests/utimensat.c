#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[])
{
	int dirfd, flags = 0, ret, symfd;
	mode_t mode = 0;
	char *buf = "Hello";
	ssize_t written;
	struct timespec times[2];

	if (argc != 3) {
		printf("Usage:%s <dir> <file-relative-path>\n", argv[0]);
		exit(1);
	}

	dirfd = open(argv[1], O_RDONLY);
	if (dirfd == -1) {
		fprintf(stderr, "Failed to open dir %s:%s\n", argv[1],
				strerror(errno));
	}

	symfd = open(argv[2], O_NOFOLLOW | O_PATH);
	if (symfd == -1) {
		fprintf(stderr, "Failed to open file %s with O_PATH:%s\n",
				argv[2], strerror(errno));
	}
	
	ret = utimensat(dirfd, argv[2], NULL, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
//	ret = utimensat(symfd, "", NULL, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);

//	ret = syscall(__NR_utimensat, symfd, NULL, NULL, 0);

//	ret = utimensat(AT_FDCWD, "", NULL, 0);
	if (ret == -1) {
		fprintf(stderr, "utimensat() failed:%s\n", strerror(errno));
	}
	close(dirfd);
	close(symfd);
}
