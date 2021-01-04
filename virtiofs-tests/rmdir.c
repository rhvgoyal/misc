#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Open a file, unlink file and then rmdir without closing fd yet */
int main(int argc, char *argv[])
{
	int fd, ret;
	char *file_path;

	if (argc != 2) {
		printf("Usage:%s <dir-to-remove>\n", argv[0]);
		exit(1);
	}

	ret = asprintf(&file_path, "%s/foo.txt", argv[1]);
	if (ret == -1) {
		fprintf(stderr, "Failed asprintf(%s/foo.txt):%s errno=%d\n", argv[1], strerror(errno), errno);
		exit(1);
	}

	/* Create and open a file foo.txt in dir */
	fd = open(file_path, O_RDWR | O_CREAT);
	if (fd == -1) {
		fprintf(stderr, "Failed to open path %s:%s, errorno=%d\n", file_path, strerror(errno), errno);
		free(file_path);
		exit(1);
	}

	/* unlink foo.txt and then remove dir */
	ret = unlink(file_path);
	if (ret == -1) {
		fprintf(stderr, "Failed to unlink path %s:%s, errorno=%d\n", file_path, strerror(errno), errno);
		free(file_path);
		exit(1);
	}

	ret = rmdir(argv[1]);
	if (ret == -1) {
		fprintf(stderr, "Failed to rmdir directory %s:%s, errorno=%d\n", argv[1], strerror(errno), errno);
		free(file_path);
		exit(1);
	}

	printf("Directory %s successfully removed\n", argv[1]);
	free(file_path);
	close(fd);
}
