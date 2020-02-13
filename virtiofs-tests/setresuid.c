#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv) {
	printf("cap_setuid and cap_setgid: %d\n",
		prctl(PR_CAPBSET_READ, CAP_SETUID|CAP_SETGID, 0, 0, 0));
	printf(" %s\n", cap_to_text(cap_get_file(argv[0]), NULL));
	printf(" %s\n", cap_to_text(cap_get_proc(), NULL));
	if (setresuid(0, 0, 0)) {
		printf("setresuid(): %s\n", strerror(errno));
		exit(1);
	}
	execve("/bin/sh", NULL, NULL);
}
