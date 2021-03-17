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
	struct file_handle *fh;
	size_t fh_size = sizeof(struct file_handle) + MAX_HANDLE_SZ;
	int mount_id;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fh = malloc(fh_size);
	if (!fh) {
		fprintf(stderr, "Failed to allocate space for file handle %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}
	memset(fh, 0, fh_size);
	fh->handle_bytes = MAX_HANDLE_SZ;

	ret = name_to_handle_at(AT_FDCWD, argv[1], fh, &mount_id, 0);
	if (ret) {
		fprintf(stderr, "name_to_handle_at(%s) failed:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	fd = open_by_handle_at(AT_FDCWD, fh, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "open_by_handle_at(%s) failed:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	printf("Successfully opened file. mount_id=%d fd=%d\n", mount_id, fd);
	printf("Press a key to continue.\n");
	getchar();

	close(fd);
}
