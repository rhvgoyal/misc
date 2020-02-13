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
	int fd, flags = 0, ret;
	mode_t mode = 0;
	void *map_addr;
	unsigned long requested_addr;
	size_t map_length = 0x4096;
	char *endptr;
	char buf_in[16];
	char *buf_out = "Hello World";
	struct stat st_buf;
	void *cur_ptr;

	if (argc < 2  || argc > 2) {
		printf("Usage:%s <file-to-mmap>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
		exit(1);
	}

	map_addr = mmap(NULL, map_length, PROT_READ, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	/* Read few bytes */
	cur_ptr = map_addr;
	printf("Reading a string at address 0x%p\n", cur_ptr);
	memcpy(buf_in, cur_ptr, 10);
	buf_in[10] = '\0';
	printf("First 10 chars from file are [%s]\n", buf_in);

	/* upgrade mapping to writable and Write few bytes */
	map_addr = mmap(map_addr, map_length, PROT_READ | PROT_WRITE,
			MAP_SHARED| MAP_FIXED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "remapping with PROT_WRITE failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	cur_ptr = map_addr;
	printf("Writing a string at address 0x%p\n", cur_ptr);
	memcpy(cur_ptr, buf_out, strlen(buf_out) + 1);

	/* Read few bytes */
	memcpy(buf_in, cur_ptr, 10);
	buf_in[10] = '\0';
	printf("First 10 chars from file are [%s]\n", buf_in);
	ret = msync(map_addr, map_length, MS_SYNC);
	if (ret == -1) {
		fprintf(stderr, "msync() failed %s, errorno=%d\n",
				strerror(errno), errno);
		exit(1);	
	}
	munmap(map_addr, map_length);
	close(fd);
}
