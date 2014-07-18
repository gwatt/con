
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


/* Maps baudrates to the B[0-9]+ values defined in termios.h */
int baudval(int baudrate);

int con(int fd);

void sigio_handler(int signum, siginfo_t *si, void *context);

int main(int argc, char *argv[]) {
	int baud;
	int fd;
	int retval = EXIT_FAILURE;
	struct sigaction sa;
	struct termios o, n;

	sa.sa_sigaction = &sigio_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGIO, &sa, NULL) != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		goto _return;
	}

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage:\ncon /path/to/file [baudrate]\n");
		goto _return;
	}
	fd = open(argv[1], O_RDWR, O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		goto _return;
	}
	if (isatty(fd)) {
		tcgetattr(fd, &o);
		n.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
		n.c_iflag = IGNPAR | ICRNL;
		n.c_oflag = 0;
		n.c_lflag = ICANON;
		n.c_cc[VMIN] = 1;
		n.c_cc[VTIME] = 0;
		if (argc == 3) {
			if (cfsetspeed(&n, baudval(atoi(argv[2]))) != 0) {
				fprintf(stderr, "%s\n", strerror(errno));
				goto _close_fd;
			}
		} else {
			cfsetospeed(&n, cfgetospeed(&o));
			cfsetispeed(&n, cfgetispeed(&n));
		}
		tcflush(fd, TCIOFLUSH);
		tcsetattr(fd, TCSANOW, &n);
	}

	retval = con(fd);

_reset_tty:
	if (isatty(fd)) {
		tcflush(fd, TCIOFLUSH);
		tcsetattr(fd, TCSANOW, &o);
	}

_close_fd:
	close(fd);

_return:
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
	} bauds[] = {
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
