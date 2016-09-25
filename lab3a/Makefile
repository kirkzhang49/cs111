CC=gcc
CFLAGS=-Wall -Wextra
USER=lab3a-804296230
SRC=lab3a.c README Makefile

.PHONY: default,clean,dist

default: lab3a.c
	$(CC) $(CFLAGS) -o lab3a lab3a.c

clean:
	rm -fr lab3a *.csv

dist: $(SRC)
	tar -czvf $(USER).tar.gz $(SRC)