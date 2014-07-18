
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


/* Maps baudrates to the B[0-9]+ values defined in termios.h */
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
#define B(x) {B##x, x}
	static const struct {
		int val;
		int rate;
	} bauds = {
		B(0), B(50), B(75), B(110), B(134), B(200), B(300), B(600), B(1200),
		B(1800), B(2400), B(4800), B(9600), B(19200), B(38400), B(57600),
		B(115200), B(230400), B(460800), B(500000), B(576000), B(921600),
		B(1000000), B(1152000), B(1500000), B(2000000), B(2500000), B(3000000),
		B(3500000), B(4000000), {-1, -1}
	};
	int i;
	int baud = -1;
	for (i = 0; bauds[i].val != -1; i++) {
		if (bauds[i].rate == baudrate) {
			baud = bauds[i].val;
			break;
		}
	}

	return baud;
}

void sigio_handler(int signum, siginfo_t *si, void *context) {
}
