#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0;
	mode_t mode = 0;
	char *buf = "Hello";
	int buf_len = strlen(buf);
	ssize_t written = 0;
	int nr_writes = 2;
	int i;
	struct timeval tv_start, tv_end, tv_elapsed;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n",
			argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Overwrite and Extend file in a loop */		
	gettimeofday(&tv_start, NULL);
	for (i = 0; i < nr_writes; i++) {
		written = write(fd, buf, buf_len);
		if (written == -1) {
			fprintf(stderr, "Failed to write %d bytes:%s,"
				" errorno=%d\n", strlen(buf), strerror(errno),
				errno);
			exit(1);
		}
		if (written < buf_len) {
			fprintf(stderr, "Wrote %d bytes. Requested %d bytes.\n",written, buf_len);
			exit(1);
		}
		lseek(fd, -1, SEEK_END);
	}
	gettimeofday(&tv_end, NULL);
	timersub(&tv_end, &tv_start, &tv_elapsed);	
	printf("%d extending writes took %d secs and %d us\n", nr_writes, tv_elapsed.tv_sec, tv_elapsed.tv_usec);
	close(fd);
}
