#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Try to read a file after truncate on host. In that case guest still
 * has valid i_size but host can't map the page being read as it has been
 * truncated. Ideally host should inject exception into guest with page
 * fault error and guest memcpy_safe() should return with error.
 */
int main(int argc, char *argv[])
{
	int fd, flags = 0;
	mode_t mode = 0;
	char buf[16];
	ssize_t ret;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}


	/* Write few bytes */
	strcpy(buf, "Hello World");
	ret = pwrite(fd, buf, strlen(buf), 0);
	if (ret == -1) {
		fprintf(stderr, "Failed to write file %s:%s, errorno=%d\n",
			argv[0], strerror(errno), errno);
		exit(1);
	}

	printf("Wrote few bytes to file %s. Truncate file on host and press"
	       " any key to continue.\n");
	getchar();

	/* Read few bytes */
	memset(buf, 0, 16);
	ret = pread(fd, buf, 15, 0);
	if (ret == -1) {
		fprintf(stderr, "Failed to read %d bytes:%s, errorno=%d\n", 15, strerror(errno), errno);
	} else {
		printf("Read from truncated region returned=%d buf=%s\n", ret, buf);
	}

	close(fd);
}
