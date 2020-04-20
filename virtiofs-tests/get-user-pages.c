#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/*
 * mmap() a dax file and read/write into that address range. This should
 * trigger kernel doing get_user_pages() in that range.
 */

int main(int argc, char *argv[])
{
	int fd, ret;
	void *map_addr;
	size_t map_length = 4096, file_length = 4096 * 1024;
	char *test_string = "Hello World";

	if (argc != 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Truncate file to 4MB lenth. First page we will mmap() and we will
	 * read/write to/from second page. Keep second page after 2MB offset
	 * so that separate dax mapping ranges are used for mmap() and
	 * read/write
	 */
	ret = ftruncate(fd, file_length);
	if (ret == -1) {
		fprintf(stderr, "Failed to truncate file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Write a string to second page at 2MB offset*/
	ret = pwrite(fd, test_string, strlen(test_string) + 1, 2048 * 1024);
	if (ret == -1) {
		fprintf(stderr, "Failed to write a string to file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Map first page */
	map_addr = mmap(NULL, map_length, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr,
	       map_length);

	/* Read string from 2MB offset into mmapped area */
	ret = pread(fd, map_addr, strlen(test_string) + 1, 2048 * 1024);
	if (ret == -1) {
		fprintf(stderr, "Failed to read() a string from file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	ret = msync(map_addr, map_length, MS_SYNC);
	if (ret == -1) {
		fprintf(stderr, "msync() failed %s, errorno=%d\n",
				strerror(errno), errno);
		exit(1);
	}

	printf("First 10 chars from file are [%s]\n", map_addr);
	munmap(map_addr, map_length);
	close(fd);
}
