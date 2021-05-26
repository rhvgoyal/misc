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
	struct statx statxbuf;

	if (argc != 2) {
		printf("Usage:%s <file-to-stat>\n", argv[0]);
		exit(1);
	}

	flags = AT_NO_AUTOMOUNT;
	ret = statx(AT_FDCWD, argv[1], flags, STATX_ALL, &statxbuf);
	if (ret == -1) {
		fprintf(stderr, "Failed to statx file %s:%s, errorno=%d\n",
			argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Print few elements */
	printf("stx_dev_major=0x%x stx_dev_minor=0x%x\n",
		statxbuf.stx_dev_major, statxbuf.stx_dev_minor);
	printf("stx_ino=%lu\n", statxbuf.stx_ino);
	printf("st_nlink=%ld mode=0x%x\n", statxbuf.stx_nlink, statxbuf.stx_mode);

#if defined(STATX_MNT_ID)
	if (statxbuf.stx_mask & STATX_MNT_ID)
		printf("stx_mnt_id=%ld\n", statxbuf.stx_mnt_id);
#endif
}
