
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define F_SETSIG	10

/* Maps baudrates to the B[0-9]+ values defined in termios.h */
int baudval(int baudrate);

int con(int fd);
int con_tty(int fd, int baud);

void sigio_handler(int signum, siginfo_t *si, void *context);

int main(int argc, char *argv[]) {
	int baud = -1;
	int fd = -1;
	int retval = EXIT_FAILURE;
	struct sigaction sa;

	sa.sa_sigaction = &sigio_handler;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sigfillset(&sa.sa_mask);
	if (sigaction(SIGIO, &sa, NULL) != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		goto _return;
	}

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage:\ncon /path/to/file [baudrate]\n");
		goto _return;
	}
	if (argc == 3) baud = atoi(argv[2]);
	fd = open(argv[1], O_RDWR, O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		goto _return;
	}
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, FASYNC);
	fcntl(fd, F_SETSIG, SIGIO);
	if (isatty(fd)) con_tty(fd, baud);
	else retval = con(fd);

_close_fd:
	close(fd);

_return:
	return retval;
}

int con_tty(int fd, int baud) {
	int retval = EXIT_FAILURE;
	struct termios n, o;
	if (tcgetattr(fd, &o) != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		goto _return;
	}
	n.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
	n.c_iflag = IGNPAR;
	n.c_oflag = 0;
	n.c_lflag = ICANON;
	n.c_cc[VMIN] = 1;
	n.c_cc[VTIME] = 0;
	baud = baudval(baud);
	if (baud >= 0) cfsetspeed(&n, baud);
	else {
		cfsetispeed(&n, cfgetispeed(&o));
		cfsetospeed(&n, cfgetospeed(&o));
	}
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &n);

	retval = con(fd);
_reset_tty:
	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &o);
_return:
	return retval;
}

int con(int fd) {
	char buf[256];
	int c;
	int retval = EXIT_FAILURE;

	do {
		c = read(STDIN_FILENO, buf, sizeof buf);
		if (c == -1) {
			if (errno == EINTR) continue;
			else goto _return;
		}
		while (c > 0 && isspace(buf[c-1])) c--;
		write(fd, buf, c);
	} while (c != 0);
	retval = EXIT_SUCCESS;
_return:
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
	char buf[256];
	int c;

	if (si->si_code == POLL_IN || si->si_code == POLL_MSG || si->si_code == POLL_PRI) {
		do {
			c = read(si->si_fd, buf, sizeof buf);
			write(STDOUT_FILENO, buf, c);
		} while (c == sizeof buf);
	}
}
