#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#define PAGE_SIZE	4096

void sigbus_handler(int signum, siginfo_t *siginfo, void *ptr)
{
	printf("sigbus_sigaction() called. signum=%d, si_addr=0x%lx si_code=%d. Exiting\n", signum, siginfo->si_addr, siginfo->si_code);
	exit(1);
}

int register_sigaction(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_flags |= SA_SIGINFO;
	sigact.sa_sigaction = sigbus_handler;

	if (sigaction(SIGBUS, &sigact, NULL) == -1) {
		printf("sigaction(SIGBUS) failed. err=%d:%s. Exiting.\n", errno,
			strerror(errno));
		exit(1);
	}
}

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
	int key;

	if (argc < 2  || argc > 3) {
		printf("Usage:%s <file-to-mmap> [<addr-to-map-at-in-hex>]\n", argv[0]);
		exit(1);
	}

	if (argc == 3) {
		errno = 0;
		requested_addr = strtoul(argv[2], &endptr, 16);
		if (errno) {
			fprintf(stderr, "strtoul(%s) failed. %s\n", argv[2], strerror(errno));
			exit(1);
		}
	}

	register_sigaction();

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);

        if(fstat(fd, &st_buf) == -1) {
                fprintf(stderr, "fstat failed %s, errorno=%d\n", strerror(errno), errno);
                exit(1);
        }

	map_length = st_buf.st_size;
	if (argc == 3)
		map_addr = mmap((void*)requested_addr, map_length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
	else
		map_addr = mmap(NULL, map_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map_addr == MAP_FAILED) {
		fprintf(stderr, "mmap failed %s, errorno=%d\n", strerror(errno), errno);
		exit(1);	
	}

	printf("mmap succeeded map_addr=0x%lx map_length=%lx\n", map_addr, map_length);
	/* Write few bytes */
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

	printf("Wrote to first page. Truncate file to 0 length and Enter. Will read after that and expect SIGBUS\n");
	scanf(" ");
	printf("Reading from mmap() region addr=0x%lx\n", cur_ptr);
	memcpy(buf_in, cur_ptr, 10);
	buf_in[10] = '\0';
	printf("First 10 chars from file are [%s]\n", buf_in);
	munmap(map_addr, map_length);
	close(fd);
}
