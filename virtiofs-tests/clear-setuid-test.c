#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd, ret, len;
	char *buf = "Hello";
	struct stat st;

	if (argc != 2) {
		printf("Usage:%s <file-to-create>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_ISUID);	
	if (fd == -1) {
		fprintf(stderr, "Failed to create and open file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("Created file %s with setuid bit set.\n", argv[1]);

	/* Write something to file and this should clear setuid bit if
	 * caller does not have setid capability
	 */
	len = strlen(buf);
	ret = write(fd, buf, len + 1);
	if (ret == -1) {
		fprintf(stderr, "Failed to write file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	/* Check back that setuid bit has been cleared. */
	ret = fstat(fd, &st);
	if (ret == -1) {
		fprintf(stderr, "Failed to stat file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	if (st.st_mode & S_ISUID) {
		fprintf(stderr, "Test failed. setuid bit is still set.\n");
	} else {
		fprintf(stderr, "Test succeeded. setuid bit cleared.\n");
	}
}

