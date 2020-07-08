#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Try to read a large file one page at a time. */
int main(int argc, char *argv[])
{
	int fd, flags = 0, i;
	mode_t mode = 0;
	char buf[16];
	struct stat st_buf;
	ssize_t ret;
	unsigned long nr_ranges;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	if(fstat(fd, &st_buf) == -1) {
		fprintf(stderr, "fstat failed %s, errorno=%d\n",
			strerror(errno), errno);
		exit(1);
	}

#define RANGE_SIZE      (2*1024*1024)
	nr_ranges = st_buf.st_size/RANGE_SIZE;
	printf("file_length=%ld nr_ranges=%ld range_size=%ld\n",
		st_buf.st_size, nr_ranges, RANGE_SIZE);
	for (i = 0; i < nr_ranges; i++) {
		/* Read few bytes */
		memset(buf, 0, 16);
		ret = pread(fd, buf, 15, i * RANGE_SIZE);
		if (ret == -1) {
			fprintf(stderr, "Failed to read %d bytes at page index=%d:%s, errorno=%d\n", 15, strerror(errno), i, errno);
		} else {
			printf("Read %d bytes at page index=%d buf=[%s]\n",
				ret, i, buf);
		}

		printf("Press a key\n");
		getchar();
	}
	close(fd);
}
