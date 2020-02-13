#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/xattr.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret;
	mode_t mode = 0;
	char *buf = "Hello";
	ssize_t written;
	char proc_path[128];

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_PATH | O_NOFOLLOW);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	printf("Open(%s, O_PATH | O_NOFOLLOW) succeeded.\n", argv[1]);

	sprintf(proc_path, "/proc/%d/fd/%d", getpid(), fd);
	ret = setxattr(proc_path, "trusted.foo", NULL, 0, 0);
	if (ret == -1) {
		fprintf(stderr, "setxattr(%s) failed:%s\n", strerror(errno));
		exit(1);
	}
	printf("setxattr(%s, trusted.foo) succeeded.\n");
	printf("Press a key to exit.\n");
	scanf(" ");
	close(fd);
}
