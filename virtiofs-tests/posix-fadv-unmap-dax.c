#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_SIZE 4096
#define NR_PAGES 16

void *mmap_addr;

int write_page(unsigned page_index)
{
	void *write_addr = mmap_addr + page_index * PAGE_SIZE;
	sprintf(write_addr, "This is page %d", page_index);
}

int write_pages(unsigned nr_pages)
{
	for (int i = 0; i < NR_PAGES; i++) {
		write_page(i);
	}
}

int read_page(unsigned page_index)
{
	void *read_addr = mmap_addr + page_index * PAGE_SIZE;
	printf("File contents at page index %d: %s\n", page_index,
			(char *)read_addr);
}

int read_pages(unsigned nr_pages)
{
	for (int i = 0; i < NR_PAGES; i++) {
		read_page(i);
	}
}

int main(int argc, char** argv)
{
	int fd;
	struct stat st;
	int ret;

	if (argc != 2) {
		fprintf(stderr, "%s: <file-path>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file%s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	mmap_addr = mmap(NULL, NR_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);	
	if (mmap_addr == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap file%s:%s\n", argv[1], strerror(errno));
		exit(1);
	}

	write_pages(NR_PAGES);

	/* unmap */
#define POSIX_FADV_UNMAP	8
	ret = posix_fadvise(fd, 0, NR_PAGES * PAGE_SIZE, POSIX_FADV_UNMAP);
	if (ret) {
		fprintf(stderr, "posix_fadvise() failed:%d\n", ret);
	}

	/* Re-read file contents and this should generate fault again */
	read_pages(NR_PAGES);

	/* Close file */
	munmap(mmap_addr, NR_PAGES * PAGE_SIZE);
	close(fd);
}
