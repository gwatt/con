
CC = cc
CFLAGS = -W -Wall -Wextra

.PHONY: clean

con: con.c
	$(CC) $(CFLAGS) -o $@ $^

install: con
	install -D con $(DESTDIR)/bin/con

clean:
	rm -rf con
