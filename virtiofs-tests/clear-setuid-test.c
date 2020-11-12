#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * Get a shell with CAP_FSETID dropped and run this program.
 * $ capsh --drop=cap_fsetid --
 * $ ./clear-setuid-test /mnt/virtiofs/foo.txt
 */

/* Return 1 if setuid bit is set. 0 otherwise */
int check_setuid_bit(int fd, char* filename)
{
	int ret;
	struct stat st;

	/* Check back that setuid bit has been cleared. */
	ret = fstat(fd, &st);
	if (ret == -1) {
		fprintf(stderr, "Failed to stat file %s:%s\n", filename, strerror(errno));
		exit(1);
	}

	if (st.st_mode & S_ISUID)
		return 1;

	return 0;
}

int main(int argc, char *argv[])
{
	int fd, ret, len;
	char *buf = "Hello";
	struct stat st;
	mode_t mode = S_IRUSR | S_IWUSR | S_ISUID;

	if (argc != 2) {
		printf("Usage:%s <file-to-create>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, mode);
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

	if (check_setuid_bit(fd, argv[1])) {
		fprintf(stderr, "Test failed. setuid bit is still set after write.\n");
	} else {
		fprintf(stderr, "Test succeeded. setuid bit cleared after write.\n");
	}

	/* Set setuid bit again */
	ret = fchmod(fd, mode);
	if (ret == -1) {
		fprintf(stderr, "Failed to set setuid bit on file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	/* Truncate file and that should clear setuid bit */
	ret = ftruncate(fd, 2);
	if (ret == -1) {
		fprintf(stderr, "Failed to set truncate() file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	if (check_setuid_bit(fd, argv[1])) {
		fprintf(stderr, "Test failed. setuid bit is still set after ftruncate().\n");
	} else {
		fprintf(stderr, "Test succeeded. setuid bit cleared after ftruncate().\n");
	}

	/* Set setuid bit again */
	ret = fchmod(fd, mode);
	if (ret == -1) {
		fprintf(stderr, "Failed to set setuid bit on file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	/* close fd and open with O_TRUNC and that should clear setuid */
	close(fd);
	fd = open(argv[1], O_RDWR | O_TRUNC);
	if (fd == -1) {
		fprintf(stderr, "Failed to set open(O_TRUNC) file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	if (check_setuid_bit(fd, argv[1])) {
		fprintf(stderr, "Test failed. setuid bit is still set after open(O_TRUNC).\n");
	} else {
		fprintf(stderr, "Test succeeded. setuid bit cleared after open(O_TRUNC).\n");
	}

	/* Set setuid bit again */
	ret = fchmod(fd, mode);
	if (ret == -1) {
		fprintf(stderr, "Failed to set setuid bit on file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	/*
	 * chown file and that should clear setuid bit. chown to "bin". chown
	 * always clears setuid, even if caller has CAP_FSETID.
	 */
	ret = fchown(fd, 1, 1);
	if (ret == -1) {
		fprintf(stderr, "Failed to set fchown() file %s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	if (check_setuid_bit(fd, argv[1])) {
		fprintf(stderr, "Test failed. setuid bit is still set after fchown().\n");
	} else {
		fprintf(stderr, "Test succeeded. setuid bit cleared after fchown().\n");
	}

	close(fd);
}
