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
	int ret;
	char xattr_name[64];
	char *xattr_value;
	int i = 0, xattr_len = 1024, nr_xattrs = 1024 * 1024;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	xattr_value = calloc(1, xattr_len);
	if (!xattr_value) {
		fprintf(stderr, "calloc() failed:%s\n", strerror(errno));
		exit(1);
	}

	for (i = 0; i < xattr_len - 1; i++) {
		xattr_value[i] = 'c';
	}
	xattr_value[i] = '\0';

	i = 0;
	while (1) {
		sprintf(xattr_name, "user.%d", i);
		ret = setxattr(argv[1], xattr_name, xattr_value,
				strlen(xattr_value) + 1, 0);
		if (ret == -1) {
			fprintf(stderr, "setxattr() failed. i=%d:%s\n",
				i, strerror(errno));
			exit(1);
		}

		if (i > nr_xattrs)
			break;

		i++;
	}
	printf("Wrote %d xattrs of size %d\n", i, strlen(xattr_value) + 1);
	printf("Press a key to exit.\n");
	getchar();
	free(xattr_value);
}
