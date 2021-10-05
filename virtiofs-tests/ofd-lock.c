#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

/*
 * Compile and link with -lpthread.
 * gcc -o ofd-lock -lpthread ofd-lock.c
 *
 * This program takes OFD lock on a file. It also creates a thread which
 * registers a SIGUSR1 handler. If ofd-lock process is blocked on lock,
 * and SIGUSR1 is sent to this process, that dedicated thread handles
 * the signal. As of now all that thread does is close the fd of the
 * file.
 *
 * In terminal 1, run following to take file lock.
 * $ ./ofd-lock foo.txt
 *
 * In another terminal, run following. This will block on file lock.
 * $ ./ofd-lock foo.txt
 *
 * Now, send SIGUSR1 to process in second terminal which is blocked
 * waiting for lock.
 *
 * $ kill -s SIGUSR1 <ofd-lock-pid>
 */

pthread_t pthread_id;
int file_fd;

static void * start_thread(void *arg)
{
	int *fd_ptr = arg;
	int fd = *fd_ptr;

	printf("start_thread: Started. Received fd=%d\n", fd);
	/* Wait for signal to arrive */
	pause();
	printf("start_thread: Closing fd=%d\n", fd);
	if (close(fd)) {
		fprintf(stderr, "start_thread: close() failed:%s\n", strerror(errno));
	}
	printf("start_thread: Exiting\n");
}

void sigusr1_handler(int signum, siginfo_t *siginfo, void *ptr)
{
        printf("sigusr1_handler() called. signum=%d tid=%d\n", signum,
		gettid());
}

int register_sigusr1_handler(void)
{
        struct sigaction sigact;

        memset(&sigact, 0, sizeof(sigact));
        sigact.sa_flags |= SA_SIGINFO;
        sigact.sa_sigaction = sigusr1_handler;

        if (sigaction(SIGUSR1, &sigact, NULL) == -1) {
                printf("sigaction(SIGUSR1) failed. err=%d:%s. Exiting.\n", errno,
                        strerror(errno));
                exit(1);
        }
}

void block_sigusr1(void)
{
	int ret;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (ret) {
		fprintf(stderr, "Failed to block SIGUSR1:%s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	int fd, flags = 0, ret;
	mode_t mode = 0;
	struct flock flock;

	if (argc != 2) {
		printf("Usage:%s <file-to-open>\n", argv[0]);
		exit(1);
	}

	register_sigusr1_handler();

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s:%s, errorno=%d\n", argv[0], strerror(errno), errno);
	}

	file_fd = fd;
	ret = pthread_create(&pthread_id, NULL, &start_thread, &file_fd);
	if (ret) {
		fprintf(stderr, "Failed to create thread:%s, errorno=%d\n", strerror(errno), errno);
		exit(1);
	}

	printf("Created a thread. Process pid=%d\n", getpid());

	/* Mask SIGUSR1 in main thread so that child thread handles it */
	block_sigusr1();

	/* Take ofd lock */		
	memset(&flock, 0, sizeof(flock));
	flock.l_type = F_WRLCK;
	flock.l_whence = SEEK_SET;
	flock.l_start = 0;
	flock.l_len = 0; /* Lock whole file */

	ret = fcntl(fd, F_OFD_SETLKW, &flock);
	if (ret == -1) {
		fprintf(stderr, "Failed to lock file:%s, errorno=%d\n",
			strerror(errno), errno);
	}
	printf("Acquired OFD lock on whole file.\n");
	printf("Press a key to exit.\n");
	getchar();
	close(fd);
}
