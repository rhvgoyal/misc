#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv)
{
	int ret;
	struct stat st_before, st_after;

	if (argc != 2) {
		printf("Usage: %s: <dir-to-create>\n", argv[0]);
		exit(1);
	}

	ret = stat(".", &st_before);
	if (ret == -1) {
		fprintf(stderr, "stat before failed:%s\n", strerror(errno));
		exit(1);
	}

	printf("Parent dir ctime before=%s\n", ctime(&st_before.st_ctime));

	printf("Sleeping for 1 sec.\n");
	sleep(1);
	
	ret = mkdir(argv[1], 0755);
	if (ret == -1) {
		fprintf(stderr, "Creation of directory %s failed:%s\n",
			argv[1], strerror(errno));
		exit(1);
	}

	ret = stat(".", &st_after);
	if (ret == -1) {
		fprintf(stderr, "stat after failed:%s\n", strerror(errno));
		exit(1);
	}
	printf("Successfully created directory %s\n", argv[1]);

	printf("Parent dir ctime after=%s\n", ctime(&st_after.st_ctime));

	printf("Sleeping for 10 seconds and check stat again.\n");
	sleep(10);
	ret = stat(".", &st_after);
	if (ret == -1) {
		fprintf(stderr, "stat after failed:%s\n", strerror(errno));
		exit(1);
	}
	printf("Parent dir ctime after=%s\n", ctime(&st_after.st_ctime));
}
