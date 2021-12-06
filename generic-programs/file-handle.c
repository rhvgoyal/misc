#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

/* This function has been taken from name_to_handle_at() man page */
int open_mount_path_by_id(int mount_id)
{
	char *linep;
	size_t lsize;
	char mount_path[PATH_MAX];
	int mi_mount_id, found;
	ssize_t nread;
	FILE *fp;

	fp = fopen("/proc/self/mountinfo", "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen(/proc/self/mountinfo) failed:%s\n",
			strerror(errno));
		exit(1);
	}

	found = 0;
	linep = NULL;
	while (!found) {
		nread = getline(&linep, &lsize, fp);
		if (nread == -1)
			break;

		nread = sscanf(linep, "%d %*d %*s %*s %s",
				&mi_mount_id, mount_path);
		if (nread != 2) {
			fprintf(stderr, "Bad sscanf()\n");
			exit(EXIT_FAILURE);
		}
		if (mi_mount_id == mount_id)
			found = 1;
	}
	free(linep);

	fclose(fp);

	if (!found) {
		fprintf(stderr, "Could not find mount point\n");
		exit(EXIT_FAILURE);
	}

	return open(mount_path, O_RDONLY);
}

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret, mount_fd;
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

	printf("name_to_handle_at() succeeded\n");

	mount_fd = open_mount_path_by_id(mount_id);

	fd = open_by_handle_at(mount_fd, fh, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "open_by_handle_at(%s) failed:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	printf("Successfully opened file. mount_id=%d fd=%d\n", mount_id, fd);
	printf("Press a key to continue.\n");
	getchar();

	close(fd);
}
