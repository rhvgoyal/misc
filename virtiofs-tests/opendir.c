#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0;
	mode_t mode = 0;
	char *buf = "Hello";
	ssize_t written;
	DIR *dirp;

	if (argc != 2) {
		printf("Usage:%s <dir-to-open>\n", argv[0]);
		exit(1);
	}

	dirp = opendir(argv[1]);
	if (!dirp) {
		fprintf(stderr, "Failed to open directory %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	closedir(dirp);
}
