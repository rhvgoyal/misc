#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret, i;
	void *map_addr;
	size_t file_length = 8192;
	char *buf_out = "a";
	char *cur_ptr, c, ch;

	if (argc != 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	/* Map a page private and write some data. This should trigger cow
	 * Make sure range reclaim does not reclaim cowed page.
	 */
	map_addr = mmap(NULL, file_length, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	/* Write something to page to create a cow page */
	cur_ptr = map_addr;
	for (i = 0; i < file_length; i++) {
		cur_ptr[i] = 'b';
	}
	
	printf("Created COW page. Press a key to continue\n");
	scanf("%c", &ch);

	/*
	 * Read whole file if any of the character is different than b, 
	 * its a corruption
	 */
	cur_ptr = map_addr;
	for (i = 0; i < file_length; i++) {
		c = cur_ptr[i];
		if (c != 'b') {
			printf("Error. Expected %c got %c. i=%d\n", 'b', c, i);
			exit(1);
		}
		if (i % 10 == 0) {
			usleep(10);
		}
	}

	munmap(map_addr, file_length);
	close(fd);
}
