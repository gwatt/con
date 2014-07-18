
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int baudval(int baudrate);

int con(int fd);

void sigio_handler(int signum, siginfo_t *si, void *context);

int main(int argc, char *argv[]) {
	int retval = EXIT_FAILURE;

	return retval;
}

int con(int fd) {
	int retval = EXIT_FAILURE;

	return retval;
}

int baudval(int baudrate) {
	int baud = -1;

	return baud;
}

void sigio_handler(int signum, siginfo_t *si, void *context) {
}
