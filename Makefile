CC      = gcc
CFLAGS  = -Wall -Wextra -g
PREFIX  = /usr/local

all: pwd_79_17 lab5_2 lab5_3

pwd_79_17: pwd_79_17.c
	$(CC) $(CFLAGS) -o $@ $<

lab5_2: lab5_2.c
	$(CC) $(CFLAGS) -rdynamic -o $@ $<

lab5_3: lab5_3.c
	$(CC) $(CFLAGS) -o $@ $<

install: pwd_79_17
	sudo install -m 755 pwd_79_17 $(PREFIX)/bin/pwd_79_17

uninstall:
	sudo rm -f $(PREFIX)/bin/pwd_79_17

clean:
	rm -f pwd_79_17 lab5_2 lab5_3

.PHONY: all install uninstall clean