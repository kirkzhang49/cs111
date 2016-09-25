CC=g++
CFLAGS=-Wall -Wextra  -std=c++11
SRC=lab3b.cpp Makefile README
USR=lab3b-804296230


.PHONY: default,clean,run,dist

default: lab3b.cpp
	$(CC) $(CFLAGS) -o lab3b lab3b.cpp

clean:
	rm -fr lab3b lab3b_check.txt

run: lab3b 
	./lab3b

dist: $(SRC)
	tar -czvf $(USR).tar.gz $(SRC)